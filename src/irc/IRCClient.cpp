#include "IRCClient.h"

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
