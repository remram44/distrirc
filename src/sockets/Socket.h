#ifndef SOCKET_H
#define SOCKET_H

#include <cstdio>
#include <cstring>           /* For memset() */
#include <exception>
#include <set>
#include <sstream>

#ifdef __WIN32__
    #include <winsock2.h>

    typedef int socklen_t;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/param.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <signal.h>
    #include <netdb.h>
    #include <errno.h>
    #include <unistd.h>

    typedef int SOCKET;
#endif


/*==============================================================================
 * Exceptions.
 *
 * Class SocketError and subclasses.
 */

 /**
 * An error from this module.
 *
 * Base class for subclassing.
 */
class SocketError : public std::exception {
public:
    virtual const char *what() = 0;
};

/**
 * Fatal error.
 *
 * An error that really shouldn't happen and is not caused by the use the
 * application makes of this module. If such an exception is thrown, we are in
 * big trouble, and this module is probably unusable. This can come from an
 * error or a limit from the OS (firewall?).
 */
class SocketFatalError : public SocketError {
public:
    const char *what();
};

/**
 * Host cannot be found.
 *
 * If the hostname we want to connect to cannot be resolved to an IP address.
 */
class SocketUnknownHost : public SocketError {
public:
    const char *what();
};

/**
 * Connection refused.
 *
 * If the peer we want to reach exists but actively informs us that it won't
 * accept a connection on the given port number.
 */
class SocketConnectionRefused : public SocketError {
public:
    const char *what();
};

/**
 * Connection closed.
 *
 * If the connection has been closed and thus cannot be used for communication.
 */
class SocketConnectionClosed : public SocketError {
public:
    const char *what();
};

/**
 * Impossible to grab the requested port numbre.
 *
 * If we want to bind to a local port number that is already in use or that we
 * are not allowed to use.
 */
class SocketCantUsePort : public SocketError {
public:
    const char *what();
};


/*============================================================================*/

/**
 * An address on the network.
 */
class SockAddress {

public:
    enum EType {
        V4 = 0x01,
        V6 = 0x02,

        ANY_TYPE = V4 | V6
    };

    virtual EType type() const = 0;
    virtual SockAddress *clone() const = 0;

};

/**
 * An IPv4 address.
 */
class SockAddress4 : public SockAddress {

public:
    unsigned char a, b, c, d;

public:
    SockAddress4(unsigned int address);
    SockAddress4(unsigned char a, unsigned char b, unsigned char c,
            unsigned char d);

    /**
     * Returns the address as a single unsigned integer, in host byte order.
     */
    unsigned int toUint() const;

    SockAddress::EType type() const;

    SockAddress *clone() const;

};


/*============================================================================*/

class SocketSetRegistrar;

class Waitable {

public:
    virtual void RegisterSockets(SocketSetRegistrar *registrar) = 0;

};


/*============================================================================*/

/**
 * A socket.
 *
 * Base class, useful notably to SocketSets.
 */
class Socket : public virtual Waitable {

private:
    int m_iSocket;

public:
    /**
     * Base constructor.
     */
    Socket(int sock);

    /**
     * Destructor: closes the socket.
     */
    virtual ~Socket();

    bool operator==(const Socket& s);
    bool operator<(const Socket& s);

    /**
     * Waits for a change on this socket.
     *
     * @param timeout Maximum time (in milliseconds) to wait for an event. 0
     * returns immediately, and a negative value means to wait forever.
     * @return true If the socket was modified (data are available for reading,
     * someone wants to connect, the connection was closed, ...) during the
     * given delay, or false if nothing happened after this time.
     */
    bool Wait(int timeout = -1) const;

    /**
     * Accessor for the socket.
     *
     * Useful to subclasses.
     */
    inline int GetSocket() const
    {
        return m_iSocket;
    }

    /**
     * Resolves a hostname and returns the 4 bytes of its IPv4 address.
     */
    static const SockAddress *Resolve(const char *name,
            unsigned int types = SockAddress::ANY_TYPE);

    void RegisterSockets(SocketSetRegistrar *registrar);

public:
    /**
     * Initializes the module.
     */
    static void Init() throw(SocketFatalError);

    /**
     * Frees the socket: it is free()d by this module, but the file descriptor
     * is not closed. Instead, it is returned and can be used through the native
     * system calls.
     */
    static int Unlock(Socket *s);

};


/*============================================================================*/

class FDSetWrapper;

class SocketSetRegistrar {

private:
    FDSetWrapper *m_Wrapper;
    Waitable *m_Waitable;

public:
    SocketSetRegistrar(FDSetWrapper *wrapper, Waitable *obj);
    void AddSocket(Socket *sock);

};

/**
 * A group of sockets.
 *
 * By putting several sockets in a SocketSet, we can put the process to sleep
 * until something happens to either one of them.
 */
class SocketSet {

private:
    std::set<Waitable*> m_Set;

public:
    /**
     * Indicates whether a Waitable is already in this group.
     */
    bool IsSet(Waitable *obj);

    /**
     * Adds a Waitable to this group.
     */
    void Add(Waitable *obj);

    /**
     * Removes a Waitable from this group.
     *
     * @return true If this Waitable was previously in the group, false
     * otherwise.
     */
    bool Remove(Waitable *obj);

    /**
     * Removes all the sockets from this group.
     */
    void Clear();

    /**
     * Waits for a change on the sockets of this group.
     *
     * @param timeout Maximum time (in milliseconds) to wait for an event. 0
     * returns immediately, and a negative value means to wait forever.
     * @return NULL if no socket was modified, or one of the modified sockets if
     * something happened.
     */
    Waitable *Wait(int timeout = -1);

};


/*============================================================================*/

/**
 * A bidirectional network connection.
 *
 * This interface represents any type of bytestream, for instance a raw TCP
 * socket, a SSL transmission, the traversal of one or more proxies, ...
 */
class NetStream : public virtual Waitable {

public:
    virtual ~NetStream() {}

    /**
     * Sends data.
     *
     * @param data Raw data to send.
     * @param size Size (in bytes) of the data.
     */
    virtual void Send(const char *data, size_t size)
        throw(SocketConnectionClosed) = 0;

    /**
     * Receives data.
     *
     * @param bWait Indicates whether we should wait for data to arrive, if
     * nothing is initially available. If false, this method can return 0.
     * @return Number of bytes that were received.
     */
    virtual int Recv(char *data, size_t size_max, bool bWait = true)
        throw(SocketConnectionClosed) = 0;

};

#endif
