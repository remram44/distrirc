#ifndef HEADER_IRCSERVER_H
#define HEADER_IRCSERVER_H

#include "sockets/Socket.h"
#include "LineConnection.h"

class IRCServer : public Waitable {

private:
    LineConnection *m_pConnection;

public:
    IRCServer(NetStream *stream);
    IRCServer(LineConnection *connection);

    void RegisterSockets(SocketSetRegistrar *registrar);

};

#endif
