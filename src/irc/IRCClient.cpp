#include "IRCClient.h"

IRCError::IRCError(const std::string &message)
  : m_sMessage(message)
{
}

IRCError::~IRCError() throw()
{
}

const char *IRCError::what() const throw()
{
    return m_sMessage.c_str();
}

IRCClient::IRCClient(NetStream *stream)
  : m_pConnection(new LineConnection(stream))
{
}

IRCClient::IRCClient(LineConnection *connection)
  : m_pConnection(connection)
{
}

void IRCClient::RegisterSockets(SocketSetRegistrar *registrar)
{
    m_pConnection->RegisterSockets(registrar);
}
