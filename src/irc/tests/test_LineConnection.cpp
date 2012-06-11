#include <cppunit/extensions/HelperMacros.h>

#include "LineConnection.h"

#include <stdexcept>
#include <iostream>

//#define _DEBUG_FAKESTREAM

class FakeStream : public NetStream {

private:
    const char *const m_Buffer;
    const int *const m_Sizes;
    size_t m_Chunk, m_Pos, m_PosInChunk;

public:
    FakeStream(const char *to_return, const int *sizes)
      : m_Buffer(to_return), m_Sizes(sizes),
        m_Chunk(0), m_Pos(0), m_PosInChunk(0)
    {
    }

    void Send(const char *data, size_t size) throw(SocketConnectionClosed)
    {
        throw std::runtime_error("FakeStream::Send called");
    }

    int Recv(char *data, size_t size_max, bool wait = true)
            throw(SocketConnectionClosed)
    {
#ifdef _DEBUG_FAKESTREAM
        std::cerr << "FakeStream::Recv(";
#endif
        size_t pos = 0;
        while(pos < size_max)
        {
            if(m_PosInChunk == m_Sizes[m_Chunk])
            {
                m_Chunk++;
                m_PosInChunk = 0;
                break; // Return between chunks...
            }
            if(m_Sizes[m_Chunk] < 0)
            {
#ifdef _DEBUG_FAKESTREAM
                std::cerr << ") throwing SocketConnectionClosed\n";
#endif
                throw SocketConnectionClosed();
            }
            data[pos] = m_Buffer[m_Pos];
#ifdef _DEBUG_FAKESTREAM
            std::cerr << m_Buffer[m_Pos];
#endif
            pos++;
            m_Pos++;
            m_PosInChunk++;
        }
#ifdef _DEBUG_FAKESTREAM
        std::cerr << ") = " << pos << "\n";
#endif
        return pos;
    }

    void RegisterSockets(SocketSetRegistrar *registrar)
    {
        throw std::runtime_error("FakeStream::RegisterSockets called");
    }

    inline size_t bytesRead() const
    {
        return m_Pos;
    }

};

class FakeStream_Test : public CppUnit::TestFixture {

public:
    void test()
    {
        const char *data = "1234" "123" "12345";
        const int sizes[] = {4, 3, 5, -1};
        NetStream *stream = new FakeStream(data, sizes);
        char buffer[6];
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 4);
            CPPUNIT_ASSERT(std::string(buffer, 4) == "1234");
        CPPUNIT_ASSERT(stream->Recv(buffer, 0) == 0);
        CPPUNIT_ASSERT(stream->Recv(buffer, 2) == 2);
            CPPUNIT_ASSERT(std::string(buffer, 2) == "12");
        CPPUNIT_ASSERT(stream->Recv(buffer, 4) == 1);
            CPPUNIT_ASSERT(buffer[0] == '3');
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 5);
            CPPUNIT_ASSERT(std::string(buffer, 5) == "12345");
        CPPUNIT_ASSERT_THROW(stream->Recv(buffer, 1), SocketConnectionClosed);
        delete stream;
    }

    void test_zero()
    {
        const char *data = "12" "56";
        const int sizes[] = {0, 2, 0, 2, 0, -1};
        NetStream *stream = new FakeStream(data, sizes);
        char buffer[6];
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 0);
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 2);
            CPPUNIT_ASSERT(std::string(buffer, 2) == "12");
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 0);
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 2);
            CPPUNIT_ASSERT(std::string(buffer, 2) == "56");
        CPPUNIT_ASSERT(stream->Recv(buffer, 16) == 0);
        CPPUNIT_ASSERT_THROW(stream->Recv(buffer, 1), SocketConnectionClosed);
    }

    CPPUNIT_TEST_SUITE(FakeStream_Test);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(test_zero);
    CPPUNIT_TEST_SUITE_END();

};

CPPUNIT_TEST_SUITE_REGISTRATION(FakeStream_Test);

class LineConnection_Test : public CppUnit::TestFixture {

public:
    /*
     * Note that LineConnection receives to an internal buffer, so it won't be
     * receiving more that 128 bytes at once no matter what the FakeStream
     * wants to return.
     * Isn't a problem here -- we're using much smaller sizes.
     */

    void test_simple()
    {
        const char *data = "123\n\n12" "34\n" "12" "3\n" "12";
        const int sizes[] = {7, 3, 2, 2, 2, -1};
        FakeStream *stream = new FakeStream(data, sizes);
        LineConnection *conn = new LineConnection(stream);
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 7);
            CPPUNIT_ASSERT(l.size() == 2);
            std::list<std::string>::const_iterator it = l.begin();
            CPPUNIT_ASSERT(std::string("123") == *it++);
            CPPUNIT_ASSERT(std::string() == *it++);
        }
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 10);
            CPPUNIT_ASSERT(l.size() == 1);
            std::list<std::string>::const_iterator it = l.begin();
            CPPUNIT_ASSERT(std::string("1234") == *it++);
        }
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 12);
            CPPUNIT_ASSERT(l.size() == 0);
        }
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 14);
            CPPUNIT_ASSERT(l.size() == 1);
            std::list<std::string>::const_iterator it = l.begin();
            CPPUNIT_ASSERT(std::string("123") == *it++);
        }
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 16);
            CPPUNIT_ASSERT(l.size() == 0);
        }
        CPPUNIT_ASSERT_THROW(conn->readLines(), SocketConnectionClosed);
        CPPUNIT_ASSERT(stream->bytesRead() == 16);
        delete conn;
    }

    void test_crlf()
    {
        const char *data = "123\n1234\r\n123";
        const int sizes[] = {13, -1};
        FakeStream *stream = new FakeStream(data, sizes);
        LineConnection *conn = new LineConnection(stream);
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 13);
            CPPUNIT_ASSERT(l.size() == 2);
            std::list<std::string>::const_iterator it = l.begin();
            CPPUNIT_ASSERT(std::string("123") == *it++);
            CPPUNIT_ASSERT(std::string("1234") == *it++);
        }
        CPPUNIT_ASSERT_THROW(conn->readLines(), SocketConnectionClosed);
        CPPUNIT_ASSERT(stream->bytesRead() == 13);
        delete conn;
    }

    void test_binary()
    {
        const char *data = "rémi\nrampin\r\n\x01is doing fine\x01\r\n"
                "what\0ever\r\nQUIT";
        const int sizes[] = {50, -1};
        FakeStream *stream = new FakeStream(data, sizes);
        LineConnection *conn = new LineConnection(stream);
        {
            std::list<std::string> l = conn->readLines();
            CPPUNIT_ASSERT(stream->bytesRead() == 50);
            CPPUNIT_ASSERT(l.size() == 4);
            std::list<std::string>::const_iterator it = l.begin();
            CPPUNIT_ASSERT(std::string("rémi") == *it++);
            CPPUNIT_ASSERT(std::string("rampin") == *it++);
            CPPUNIT_ASSERT(std::string("\x01is doing fine\x01") == *it++);
            CPPUNIT_ASSERT(std::string("what\0ever", 9) == *it++);
        }
        CPPUNIT_ASSERT_THROW(conn->readLines(), SocketConnectionClosed);
        CPPUNIT_ASSERT(stream->bytesRead() == 50);
        delete conn;
    }

    CPPUNIT_TEST_SUITE(LineConnection_Test);
    CPPUNIT_TEST(test_simple);
    CPPUNIT_TEST(test_crlf);
    CPPUNIT_TEST(test_binary);
    CPPUNIT_TEST_SUITE_END();

};

CPPUNIT_TEST_SUITE_REGISTRATION(LineConnection_Test);
