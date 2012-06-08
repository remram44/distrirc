#ifndef TCP_H
#define TCP_H

#include "Socket.h"


/*============================================================================*/

/**
 * A connected TCP socket.
 *
 * The most simple socket, on which we can send and receive data.
 */
class TCPSocket : public NetStream, public Socket {

public:
    /**
     * Constructor from a previously connected socket.
     */
    TCPSocket(int sock);

    virtual ~TCPSocket() {}

    /**
     * Static method establishing a new connection.
     *
     * @param hote Hostname, for instance "www.debian.com".
     * @param port Port number on which to connect.
     */
    static TCPSocket *Connect(const char *host, int port)
        throw(SocketUnknownHost, SocketConnectionRefused);

    /**
     * Static method establishing a new connection.
     *
     * @param dest Destination address.
     * @param port Port number on which to connect.
     */
    static TCPSocket *Connect(const SockAddress *dest, int port)
        throw(SocketConnectionRefused);

    /**
     * Sends data.
     *
     * @param data Raw data to send.
     * @param size Size (in bytes) of the data.
     */
    virtual void Send(const char *data, size_t size) throw(SocketConnectionClosed);

    /**
     * Receives data.
     *
     * @param bWait Indicates whether we should wait for data to arrive, if
     * nothing is initially available. If false, this method can return 0.
     * @return Number of bytes that were received.
     */
    virtual int Recv(char *data, size_t size_max, bool bWait = true)
        throw(SocketConnectionClosed);

    /**
     * Gets the local port to which this socket is bound, or -1.
     */
    int GetLocalPort() const;

};


/*============================================================================*/

/**
 * A TCP server socket.
 *
 * Allows to listen on a port and accept TCP connections.
 */
class TCPServer : public Socket {

public:
    /**
     * Constructor from a socket already listening on a port.
     */
    TCPServer(int sock);

    virtual ~TCPServer() {}

    /**
     * Static method creating a server socket listening on a given port number.
     *
     * @param port Port number on which to listen for connections, or 0 for a
     * random port number.
     */
    static TCPServer *Listen(int port) throw(SocketCantUsePort);

    /**
     * Accepts one connection from a client.
     *
     * @param timeout Maximum time (in milliseconds) to wait for a connection. 0
     * returns immediately, and a negative value means to wait forever.
     * @return A socket connected to the client, or NULL if no client attempted
     * to connected after this time.
     */
    virtual TCPSocket *Accept(int timeout = 0);

    /**
     * Gets the local port to which this socket is bound, or -1.
     */
    int GetLocalPort() const;

};

#endif
