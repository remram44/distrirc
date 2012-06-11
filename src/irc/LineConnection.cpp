#include "LineConnection.h"

LineConnection::LineConnection(NetStream *stream)
  : m_pStream(stream)
{
}

LineConnection::~LineConnection()
{
    delete m_pStream;
}

std::list<std::string> LineConnection::readLines(bool wait)
            throw(SocketConnectionClosed)
{
    char buffer[128];
    int ret = m_pStream->Recv(buffer, 128, wait);
    std::list<std::string> lines;
    size_t prev_size = m_sBuffer.size();
    m_sBuffer.append(buffer, ret);
    size_t pos = m_sBuffer.find('\n', prev_size);
    while(pos != std::string::npos)
    {
        std::string result = m_sBuffer.substr(0, pos);
        m_sBuffer = m_sBuffer.substr(pos+1);
        if(result[pos-1] == '\r')
            result.resize(pos-1);
        lines.push_back(result);
        pos = m_sBuffer.find('\n', 0);
    }
    return lines;
}

void LineConnection::RegisterSockets(SocketSetRegistrar *registrar)
{
    m_pStream->RegisterSockets(registrar);
}
