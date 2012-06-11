#ifndef HEADER_LINECONNECTION_H
#define HEADER_LINECONNECTION_H

#include <string>
#include <list>

#include "sockets/Socket.h"

class LineConnection : public Waitable {

private:
    NetStream *m_pStream;
    std::string m_sBuffer;

public:
    LineConnection(NetStream *stream);
    ~LineConnection();
    std::list<std::string> readLines(bool wait = false)
            throw(SocketConnectionClosed);

    void RegisterSockets(SocketSetRegistrar *registrar);

};

#endif
