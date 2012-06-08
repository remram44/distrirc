#include "IRCServer.h"

IRCServer::IRCServer(NetStream *stream)
{
    IRCServer(new LineConnection(stream));
}

IRCServer::IRCServer(LineConnection *connection)
  : m_pConnection(connection)
{
}

void IRCServer::RegisterSockets(SocketSetRegistrar *registrar)
{
    m_pConnection->RegisterSockets(registrar);
}
