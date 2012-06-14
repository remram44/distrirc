#ifndef HEADER_LINECONNECTION_H
#define HEADER_LINECONNECTION_H

#include <string>
#include <list>

#include "sockets/Socket.h"

/**
 * A buffered stream that allows to receive full lines.
 */
class LineConnection : public Waitable {

private:
    NetStream *m_pStream;
    std::string m_sBuffer;

public:
    /**
     * Create a line-buffered connection from the given network stream.
     */
    LineConnection(NetStream *stream);
    ~LineConnection();
    /**
     * Receives data and returns the full lines that have been received.
     *
     * Might return an empty list even if wait was specified, if data was
     * received but didn't make a full line.
     * This method WILL NOT return the partial line received before the
     * connection was lost.
     */
    std::list<std::string> readLines(bool wait = false)
            throw(SocketConnectionClosed);

    void RegisterSockets(SocketSetRegistrar *registrar);

};

#endif
