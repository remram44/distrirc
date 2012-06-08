#include "TCP.h"

TCPSocket::TCPSocket(int sock)
  : Socket::Socket(sock)
{
}

TCPSocket *TCPSocket::Connect(const SockAddress *dest, int port)
    throw(SocketConnectionRefused)
{
    if(dest->type() != SockAddress::V4)
        throw SocketConnectionRefused();

    TCPSocket *sock = new TCPSocket(socket(AF_INET, SOCK_STREAM, 0));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(((SockAddress4*)dest)->toUint());
    address.sin_port = htons(port);

    memset(&(address.sin_zero), 0, 8);

    if(connect(sock->GetSocket(), (struct sockaddr*)&address,
        sizeof(address)) == -1)
    {
        throw SocketConnectionRefused();
    }

    return sock;
}

TCPSocket *TCPSocket::Connect(const char *host, int port)
    throw(SocketUnknownHost, SocketConnectionRefused)
{
    TCPSocket *sock = new TCPSocket(socket(AF_INET, SOCK_STREAM, 0));

    // Hostname resolution
    struct sockaddr_in address;
    struct hostent* h = gethostbyname(host);
    if(h == NULL)
    {
        throw SocketUnknownHost();
    }

    address.sin_family = AF_INET;
    address.sin_addr = *((struct in_addr *)h->h_addr);
    address.sin_port = htons(port);

    memset(&(address.sin_zero), 0, 8);

    if(connect(sock->GetSocket(), (struct sockaddr*)&address,
        sizeof(address)) == -1)
    {
        throw SocketConnectionRefused();
    }

    return sock;
}

void TCPSocket::Send(const char *data, size_t size)
    throw(SocketConnectionClosed)
{
    int ret = send(GetSocket(), data, size, 0);
    if(ret != (int)size)
        throw SocketConnectionClosed();
}

int TCPSocket::Recv(char *data, size_t size_max, bool bWait)
    throw(SocketConnectionClosed)
{
    if(bWait || Wait(0))
    {
        int ln = recv(GetSocket(), data, size_max, 0);
        if(ln <= 0)
            throw SocketConnectionClosed();
        else
            return ln;
    }
    else
        return 0;
}

int TCPSocket::GetLocalPort() const
{
    struct sockaddr_in address;
    socklen_t size = sizeof(address);
    if(getsockname(GetSocket(), (struct sockaddr*)&address, &size) < 0)
        return -1;
    return ntohs(address.sin_port);
}

/*============================================================================*/

TCPServer::TCPServer(int sock)
  : Socket::Socket(sock)
{
}

TCPServer *TCPServer::Listen(int port) throw(SocketCantUsePort)
{
    TCPServer *sock = new TCPServer(socket(AF_INET, SOCK_STREAM, 0));

    struct sockaddr_in sin;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if( (bind(sock->GetSocket(), (struct sockaddr*)&sin, sizeof(sin) ) == -1)
     || (listen(sock->GetSocket(), 5) == -1) )
        throw SocketCantUsePort();

    return sock;
}

TCPSocket *TCPServer::Accept(int timeout)
{
    if(Wait(timeout))
    {
        struct sockaddr_in clientsin;
        socklen_t taille = sizeof(clientsin);
        int sock = accept(GetSocket(), (struct sockaddr*)&clientsin, &taille);

        if(sock != -1)
            return new TCPSocket(sock);
        else
        {
            // Shoudln't happen! Did someone close our socket?
            throw SocketFatalError();
            return NULL;
        }
    }

    return NULL;
}

int TCPServer::GetLocalPort() const
{
    struct sockaddr_in address;
    socklen_t size = sizeof(address);
    if(getsockname(GetSocket(), (struct sockaddr*)&address, &size) < 0)
        return -1;
    return ntohs(address.sin_port);
}
