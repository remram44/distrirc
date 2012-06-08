#ifndef HEADER_LINECONNECTION_H
#define HEADER_LINECONNECTION_H

#include "sockets/Socket.h"

class LineConnection : public Waitable {

private:
    NetStream *m_pStream;
    std::string m_sBuffer;

public:
    LineConnection(NetStream *stream);
    std::string readLine(bool wait = false) throw(SocketConnectionClosed);

    void RegisterSockets(SocketSetRegistrar *registrar);

};

#endif
