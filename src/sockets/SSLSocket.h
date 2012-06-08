#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include "Socket.h"
#include "TCP.h"

#include <string>

#include <openssl/ssl.h>

class SSLError : public SocketError {
private:
    std::string m_w;
public:
    SSLError(const std::string &w);
    const char *what();
    virtual ~SSLError() throw() {}
};

class SSLConfig;

class SSLSocket {

public:
    enum EFiletype {
        PEM,
        ASN1
    };

protected:
    SSL_CTX *m_CTX;

public:
    /**
     * Initializes SSL.
     */
    static void Init() throw(SSLError);

    /**
     * Constructor.
     */
    SSLSocket(const SSLConfig &ctx) throw(SSLError);

    /**
     * Destructor.
     */
    virtual ~SSLSocket();

};

/**
 * Configuration for an SSL connection.
 *
 * It is possible to use a SSLConfig object to specify all of the parameters of
 * the SSL connection. This object is then passed when the SSL socket is
 * constructed.
 * @warning Please note that checking of the different files (certificates,
 * keys, ...) will only happen when a socket or a context is effectively
 * created.
 */
class SSLConfig {

private:
    std::string m_sCertChainFile;
    bool m_bAskForPasswd;
    std::string m_sPasswd;
    std::string m_sCAList;
    std::string m_sCipherList;
    std::string m_sCertificateFile;
    std::string m_sPrivatekeyFile;
    SSLSocket::EFiletype m_ePrivatekeyType;

public:
    SSLConfig();
    void useCertificateChainFile(const char *key);
    void askForPassword();
    void setPassword(const char *passwd = NULL);
    void loadVerifyLocations(const char *ca_list);
    void setCipherList(const char *ciphers);
    void setCertificateFilename(const char *file);
    void setPrivatekeyFilename(const char *file, SSLSocket::EFiletype type =
        SSLSocket::PEM);

    friend SSLSocket::SSLSocket(const SSLConfig &ctx) throw(SSLError);

};

class SSLServer : public SSLSocket, public TCPServer {

private:
    SSLConfig m_Config;

protected:
    SSLServer(int sock, const SSLConfig &ctx = SSLConfig())
        throw(SocketConnectionClosed, SSLError);

public:
    /**
     * Constructs a SSL server from a listening TCP socket.
     */
    static SSLServer *fromSocket(int sock, const SSLConfig &ctx = SSLConfig());

    /**
     * Constructs a SSL server from a listening TCP socket.
     *
     * @warning The given socket is destroyed.
     */
    static SSLServer *fromSocket(TCPServer *sock,
        const SSLConfig &ctx = SSLConfig());

    /**
     * Static method listening on a port.
     */
    static SSLServer *Listen(int port, const SSLConfig &ctx = SSLConfig())
        throw(SocketCantUsePort);

    /**
     * Destructor.
     */
    ~SSLServer();

    TCPSocket *Accept(int timeout = 0);

    TCPSocket *Accept(int timeout, bool askForClientCert);

};

class SSLClient : public SSLSocket, public TCPSocket {

public:
    enum ERole {
        CLIENT,
        SERVER,
        SERVER_FORCE_CERT
    };

private:
    SSL *m_SSL;
    BIO *m_BIO;

protected:
    SSLClient(int sock, ERole role, const SSLConfig &ctx = SSLConfig())
        throw(SocketConnectionClosed, SSLError);

public:
    /**
     * Constructs a secure connection from an already connected TCP socket.
     */
    static SSLClient *fromSocket(int sock, const SSLConfig &ctx = SSLConfig())
        throw(SocketConnectionClosed, SSLError);

    /**
     * Constructs a secure connection from a connected TCP socket.
     *
     * @warning The given socket is destroyed.
     */
    static SSLClient *fromSocket(TCPSocket *sock,
        const SSLConfig &ctx = SSLConfig())
        throw(SocketConnectionClosed, SSLError);

    /**
     * Static method establishing a new connection.
     *
     * @param host Hostname to connect to, for instance "www.debian.com".
     * @param port Port number to reach.
     */
    static SSLClient *Connect(const char *host, int port,
        const SSLConfig &ctx = SSLConfig())
        throw(SocketUnknownHost, SocketConnectionRefused, SSLError);

    /**
     * Static method ending the secure liaison.
     *
     * This method indicates to the other machine that we wish to end the
     * secure session, and waits for the answer to return. If the peer answers,
     * the secure session is finished and this function returns a TCPSocket
     * which can be used to communicate in a standard unsecure fashion.
     * @warning The SSLClient is destroyed.
     */
    static TCPSocket *shutdownSSL(SSLClient *c)
        throw(SocketConnectionClosed);

    /**
     * Destructor.
     */
    ~SSLClient();

    /**
     * Checks the certificate of the other machine.
     *
     * @warning Will return true if we accepted a client connection without
     * asking for a certificate!
     */
    bool checkPeerCert() const;

    /**
     * Returns the "Common Name" of the peer's certificate, to be checked (for
     * instance by comparing it to the hostname used when connecting).
     */
    std::string getPeerCertCN() const;

    /**
     * Sends data.
     *
     * @param data Raw data to send.
     * @param size Size (in bytes) of the data.
     */
    void Send(const char *data, size_t size) throw(SocketConnectionClosed);

    /**
     * Receives data.
     *
     * @param bWait Indicates whether we should wait for data to arrive, if
     * nothing is initially available. If false, this method can return 0.
     * @return Number of bytes that were received.
     */
    int Recv(char *donnees, size_t size_max, bool bWait = true)
        throw(SocketConnectionClosed);

    friend TCPSocket *SSLServer::Accept(int timeout = 0);
    friend TCPSocket *SSLServer::Accept(int timeout, bool askForClientCert);

};

#endif
