#include <cppunit/extensions/HelperMacros.h>

#include "LineConnection.h"

#include <stdexcept>
#include <iostream>

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
        std::cerr << "FakeStream::Recv(";
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
                std::cerr << ") throwing SocketConnectionClosed\n";
                throw SocketConnectionClosed();
            }
            data[pos] = m_Buffer[m_Pos];
            std::cerr << m_Buffer[m_Pos];
            pos++;
            m_Pos++;
            m_PosInChunk++;
        }
        std::cerr << ") = " << pos << "\n";
        return pos;
    }

    void RegisterSockets(SocketSetRegistrar *registrar)
    {
        throw std::runtime_error("FakeStream::RegisterSockets called");
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

/*class LineConnection_Test : public CppUnit::TestFixture {

private:
    Mod1_Truc *m_Truc;

public:
    void setUp()
    {
        m_Truc = new Mod1_Truc(12);
    }

    void tearDown()
    {
        delete m_Truc;
    }

    void testFoo()
    {
        CPPUNIT_ASSERT(m_Truc->foo() == 12);
    }

    void testBar()
    {
        CPPUNIT_ASSERT(m_Truc->bar() == 42);
    }

    CPPUNIT_TEST_SUITE(LineConnection_Test);
    CPPUNIT_TEST(testFoo);
    CPPUNIT_TEST(testBar);
    CPPUNIT_TEST_SUITE_END();

};

CPPUNIT_TEST_SUITE_REGISTRATION(LineConnection_Test);*/
