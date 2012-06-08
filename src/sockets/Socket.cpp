#include "Socket.h"

#include <map>

const char *SocketFatalError::what()
{
    return "Fatal error";
}

const char *SocketUnknownHost::what()
{
    return "Unknown host";
}

const char *SocketConnectionRefused::what()
{
    return "Connection refused";
}

const char *SocketConnectionClosed::what()
{
    return "Connection closed";
}

const char *SocketCantUsePort::what()
{
    return "Can't use port";
}


/*============================================================================*/

SockAddress4::SockAddress4(unsigned int address)
  : a(address >> 24), b(address >> 16), c(address >> 8), d(address)
{
}

SockAddress4::SockAddress4(unsigned char a_, unsigned char b_, unsigned char c_,
        unsigned char d_)
  : a(a_), b(b_), c(c_), d(d_)
{
}

unsigned int SockAddress4::toUint() const
{
    return (a << 24) | (b << 16) | (c << 8) | d;
}

SockAddress::EType SockAddress4::type() const
{
    return SockAddress::V4;
}

SockAddress *SockAddress4::clone() const
{
    return new SockAddress4(a, b, c, d);
}


/*============================================================================*/

Socket::Socket(int sock)
  : m_iSocket(sock)
{
    if(m_iSocket == -1)
        throw SocketFatalError();
}

Socket::~Socket()
{
    if(m_iSocket != -1)
    {
#ifndef __WIN32__
        shutdown(m_iSocket, SHUT_RDWR);
        close(m_iSocket);
#else
        closesocket(m_iSocket);
#endif
        m_iSocket = -1;
    }
}

bool Socket::operator==(const Socket& s)
{
    return m_iSocket == s.GetSocket();
}

bool Socket::operator<(const Socket& s)
{
    return m_iSocket < s.GetSocket();
}

void Socket::Init() throw(SocketFatalError)
{
#ifdef __WIN32__
    static bool bInit = false;
    if(!bInit)
    {
        // Initialization of WINSOCK (on Windows machines only)
        WSADATA wsa;
        if(WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
            throw SocketFatalError();
        bInit = true;
    }
#endif
}

bool Socket::Wait(int timeout) const
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET((SOCKET)m_iSocket, &fds);

    if(timeout < 0)
        select(m_iSocket + 1, &fds, NULL, NULL, NULL);
    else
    {
        timeval tv;

        tv.tv_sec = timeout/1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        select(m_iSocket + 1, &fds, NULL, NULL, &tv);
    }

    return FD_ISSET(m_iSocket, &fds);
}

void Socket::RegisterSockets(SocketSetRegistrar *registrar)
{
    registrar->AddSocket(this);
}

int Socket::Unlock(Socket *s)
{
    int sock = s->GetSocket();
    s->m_iSocket = -1;
    delete s;
    return sock;
}

// FIXME : to be updated (IPv6, ...)
const SockAddress *Socket::Resolve(const char *name, unsigned int types)
{
    if(!(types & SockAddress::V4))
        return NULL;
    struct hostent *h = gethostbyname(name);
    if(h == NULL)
        return NULL;
    else
    {
        unsigned int buf;
        memcpy(&buf, h->h_addr, sizeof(struct in_addr));
        return new SockAddress4(ntohl(buf));
    }
}


/*============================================================================*/

bool SocketSet::IsSet(Waitable *obj)
{
    std::set<Waitable*>::const_iterator it;
    it = m_Set.find(obj);
    return it != m_Set.end();
}

void SocketSet::Add(Waitable *obj)
{
    if(!IsSet(obj))
        m_Set.insert(obj);
}

bool SocketSet::Remove(Waitable *obj)
{
    std::set<Waitable*>::iterator it;
    it = m_Set.find(obj);
    if(it != m_Set.end())
    {
        m_Set.erase(it);
        return true;
    }
    else
        return false;
}

void SocketSet::Clear()
{
    m_Set.clear();
}


/*============================================================================*/

class FDSetWrapper {

private:
    fd_set fds;
    int greatest;
    std::map<Socket*, Waitable*> registered_sockets;

public:
    FDSetWrapper();
    void AddSocket(Waitable *obj, Socket *sock);
    Waitable *Wait(int timeout);

};

FDSetWrapper::FDSetWrapper()
{
    FD_ZERO(&fds);
    greatest = -1;
}

void FDSetWrapper::AddSocket(Waitable *obj, Socket *sock)
{
    int s = sock->GetSocket();
    if(s > greatest)
        greatest = s;
    registered_sockets[sock] = obj;
    FD_SET((SOCKET)s, &fds);
}

Waitable *FDSetWrapper::Wait(int timeout)
{
    if(greatest == -1)
        return NULL; // No valid socket?

    if(timeout == -1)
        select(greatest + 1, &fds, NULL, NULL, NULL);
    else
    {
        timeval tv;

        tv.tv_sec = timeout/1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        select(greatest + 1, &fds, NULL, NULL, &tv);
    }

    std::map<Socket*, Waitable*>::const_iterator it;
    it = registered_sockets.begin();
    while(it != registered_sockets.end())
    {
        if(FD_ISSET(it->first->GetSocket(), &fds))
            return it->second;
        else
            ++it;
    }

    return NULL;
}

SocketSetRegistrar::SocketSetRegistrar(FDSetWrapper *wrapper, Waitable *obj)
  : m_Wrapper(wrapper), m_Waitable(obj)
{
}

void SocketSetRegistrar::AddSocket(Socket *sock)
{
    m_Wrapper->AddSocket(m_Waitable, sock);
}

Waitable *SocketSet::Wait(int timeout)
{
    FDSetWrapper fds;

    std::set<Waitable*>::iterator it = m_Set.begin();
    while(it != m_Set.end())
    {
        SocketSetRegistrar reg(&fds, *it);
        (*it)->RegisterSockets(&reg);
        ++it;
    }

    return fds.Wait(timeout);
}
