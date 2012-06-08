#include "LineConnection.h"

LineConnection::LineConnection(NetStream *stream)
  : m_pStream(stream)
{
}

std::string LineConnection::readLine(bool wait) throw(SocketConnectionClosed)
{
    char buffer[128];
    int ret = m_pStream->Recv(buffer, 128, wait);
    size_t prev_size = m_sBuffer.size();
    m_sBuffer.append(buffer, ret);
    size_t pos = m_sBuffer.find('\n', prev_size);
    if(pos == std::string::npos)
        return std::string();
    std::string result = m_sBuffer.substr(0, pos);
    m_sBuffer = m_sBuffer.substr(pos+1);
    if(result[pos-1] == '\r')
        result.resize(pos-1);
    return result;
}

void LineConnection::RegisterSockets(SocketSetRegistrar *registrar)
{
    m_pStream->RegisterSockets(registrar);
}
