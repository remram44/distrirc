#include "SSLSocket.h"

#include <iostream>

SSLError::SSLError(const std::string &w)
  : m_w(w)
{
}

const char *SSLError::what()
{
    return m_w.c_str();
}

void SSLSocket::Init() throw(SSLError)
{
    bool bInit = false;
    if(!bInit)
    {
        if(!SSL_library_init())
            throw SSLError("Couldn't initialize OpenSSL");
        SSL_load_error_strings();
        bInit = true;
    }
}

SSLConfig::SSLConfig()
  : m_bAskForPasswd(false)
{
}

void SSLConfig::useCertificateChainFile(const char *key)
{
    if(key != NULL)
        m_sCertChainFile = key;
    else
        m_sCertChainFile = "";
}

void SSLConfig::askForPassword()
{
    m_bAskForPasswd = true;
}

void SSLConfig::setPassword(const char *passwd)
{
    m_bAskForPasswd = false;
    if(passwd != NULL)
        m_sPasswd = passwd;
    else
        m_sPasswd = "";
}

void SSLConfig::loadVerifyLocations(const char *ca_list)
{
    if(ca_list != NULL)
        m_sCAList = ca_list;
    else
        m_sCAList = "";
}

void SSLConfig::setCipherList(const char *ciphers)
{
    if(ciphers != NULL)
        m_sCipherList = ciphers;
    else
        m_sCipherList = "";
}

void SSLConfig::setCertificateFilename(const char *file)
{
    if(file != NULL)
        m_sCertificateFile = file;
    else
        m_sCertificateFile = "";
}

void SSLConfig::setPrivatekeyFilename(const char *file,
    SSLSocket::EFiletype type)
{
    if(file != NULL)
        m_sPrivatekeyFile = file;
    else
        m_sPrivatekeyFile = "";
    m_ePrivatekeyType = type;
}

static int password_cb(char *buf, int num, int rwflag, void *userdata)
{
    std::string *pass = (std::string*)userdata;
    if(num < (signed int)(pass->size()+1)) // cast to prevent warning
        return 0;

    strcpy(buf, pass->c_str());
    delete pass;
    return pass->size();
}

static int password_ask_cb(char *buf, int num, int rwflag, void *userdata)
{
    char pass1[256], pass2[256];
    std::cerr << "A password is requested to open a certificate for a "
        << (rwflag?"write":"read") << " operation\n";
    bool ok = rwflag == 0;
    do {
        std::cerr << "Password: ";
        std::cin.get(pass1, 256);
        if(rwflag)
        {
            std::cerr << "Verify password: ";
            std::cin.get(pass2, 256);
            if(std::string(pass1) == pass2)
                ok = true;
            else
                std::cerr << "Passwords did not match !\n";
        }
    }
    while(!ok);

    strncpy(buf, pass1, num);
    buf[num - 1] = '\0';
    return strlen(buf);
}

SSLSocket::SSLSocket(const SSLConfig &ctx) throw(SSLError)
{
    m_CTX = SSL_CTX_new(SSLv23_method());
    if(m_CTX == NULL)
        throw SSLError("Can't create a SSL_CTX object");
    if(ctx.m_sCertChainFile != "")
    {
        if(!SSL_CTX_use_certificate_chain_file(m_CTX,
                ctx.m_sCertChainFile.c_str()))
            throw SSLError("Can't read trusted certificate file");
    }
    if(ctx.m_bAskForPasswd)
        SSL_CTX_set_default_passwd_cb(m_CTX, password_ask_cb);
    else if(ctx.m_sPasswd != "")
    {
        SSL_CTX_set_default_passwd_cb_userdata(m_CTX, (void*)&ctx.m_sPasswd);
        SSL_CTX_set_default_passwd_cb(m_CTX, password_cb);
    }
    if(ctx.m_sCertChainFile != "")
    {
        if(!SSL_CTX_use_PrivateKey_file(m_CTX, ctx.m_sCertChainFile.c_str(),
                SSL_FILETYPE_PEM))
            throw SSLError("Can't read trusted certificate file");
    }
    if(ctx.m_sCAList != "")
    {
        if(!SSL_CTX_load_verify_locations(m_CTX, ctx.m_sCAList.c_str(), 0))
            throw SSLError("Can't read CA list");
#if OPENSSL_VERSION_NUMBER < 0x00905100L
        SSL_CTX_set_verify_depth(m_CTX, 1);
#endif
    }
    if(ctx.m_sCipherList != "")
        SSL_CTX_set_cipher_list(m_CTX, ctx.m_sCipherList.c_str());
    if(ctx.m_sCertificateFile != "")
    {
        if(!SSL_CTX_use_certificate_file(m_CTX, ctx.m_sCertificateFile.c_str(),
                SSL_FILETYPE_PEM))
            throw SSLError("Can't read own certificate file");
    }
    if(ctx.m_sPrivatekeyFile != "")
    {
        int type;
        switch(ctx.m_ePrivatekeyType)
        {
        case SSLSocket::PEM:
            type = SSL_FILETYPE_PEM;
            break;
        case SSLSocket::ASN1:
            type = SSL_FILETYPE_ASN1;
            break;
        }
        if(!SSL_CTX_use_PrivateKey_file(m_CTX, ctx.m_sPrivatekeyFile.c_str(),
                SSL_FILETYPE_PEM))
            throw SSLError("Can't load private key file");
    }
}

SSLSocket::~SSLSocket()
{
    SSL_CTX_free(m_CTX);
}

SSLClient::SSLClient(int sock, SSLClient::ERole role, const SSLConfig &ctx)
    throw(SocketConnectionClosed, SSLError)
  : SSLSocket(ctx), TCPSocket::TCPSocket(sock)
{
    m_SSL = SSL_new(m_CTX);
    SSL_set_mode(m_SSL, SSL_MODE_AUTO_RETRY);
    m_BIO = BIO_new_socket(GetSocket(), BIO_NOCLOSE);
    SSL_set_bio(m_SSL, m_BIO, m_BIO);
    switch(role)
    {
    case SSLClient::CLIENT:
        if(SSL_connect(m_SSL) < 0)
            throw SSLError("Connect error");
        break;
    case SSLClient::SERVER_FORCE_CERT:
        SSL_set_verify(m_SSL,
                SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                0);
    case SSLClient::SERVER:
        {
            if(SSL_accept(m_SSL) < 0)
                throw SSLError("Accept error");
        }
        break;
    }
}

SSLClient *SSLClient::fromSocket(int sock, const SSLConfig &ctx)
    throw(SocketConnectionClosed, SSLError)
{
    return new SSLClient(sock, SSLClient::CLIENT, ctx);
}

SSLClient *SSLClient::fromSocket(TCPSocket *sock, const SSLConfig &ctx)
    throw(SocketConnectionClosed, SSLError)
{
    return new SSLClient(Socket::Unlock(sock), SSLClient::CLIENT, ctx);
}

SSLClient *SSLClient::Connect(const char *host, int port, const SSLConfig &ctx)
    throw(SocketUnknownHost, SocketConnectionRefused, SSLError)
{
    return new SSLClient(
        Socket::Unlock(TCPSocket::Connect(host, port)),
        SSLClient::CLIENT,
        ctx);
}

TCPSocket *SSLClient::shutdownSSL(SSLClient *c) throw(SocketConnectionClosed)
{
    switch(SSL_shutdown(c->m_SSL)) // Sends a shutdown notice
    {
    case 0:
        if(SSL_shutdown(c->m_SSL) != 1) // Waits for the answer
            throw SSLError("The peer didn't answer our \"close notify\"");
        break;
    case 1: // Ok
        break;
    default:
        throw SSLError("SSL_shutdown returned -1");
        break;
    }

    return new TCPSocket(Socket::Unlock(c));
}

bool SSLClient::checkPeerCert() const
{
    if(SSL_get_verify_result(m_SSL) != X509_V_OK)
        return false;
    else
        return true;
}

std::string SSLClient::getPeerCertCN() const
{
    X509 *peer = SSL_get_peer_certificate(m_SSL);
    if(peer == NULL)
        return "";
    char peer_CN[256];
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer),
            NID_commonName, peer_CN, 256);
    return peer_CN;
}

void SSLClient::Send(const char *data, size_t size)
    throw(SocketConnectionClosed)
{
    SSL_write(m_SSL, data, size);
}

int SSLClient::Recv(char *data, size_t size_max, bool bWait)
    throw(SocketConnectionClosed)
{
    if(bWait || Wait(0))
    {
        int ln = SSL_read(m_SSL, data, size_max);
        if(ln <= 0)
            throw SocketConnectionClosed();
        else
            return ln;
    }
    else
        return 0;
}

SSLClient::~SSLClient()
{
    SSL_shutdown(m_SSL);
    BIO_free_all(m_BIO);
}

SSLServer::SSLServer(int sock, const SSLConfig &ctx)
    throw(SocketConnectionClosed, SSLError)
  : SSLSocket(ctx), TCPServer::TCPServer(sock), m_Config(ctx)
{
}

TCPSocket *SSLServer::Accept(int timeout)
{
    TCPSocket *sock = TCPServer::Accept(timeout);
    if(sock == NULL)
        return NULL;
    else
        return new SSLClient(Socket::Unlock(sock), SSLClient::SERVER, m_Config);
}

TCPSocket *SSLServer::Accept(int timeout, bool askForClientCert)
{
    TCPSocket *sock = TCPServer::Accept(timeout);
    if(sock == NULL)
        return NULL;
    else if(askForClientCert)
        return new SSLClient(Socket::Unlock(sock), SSLClient::SERVER_FORCE_CERT,
                m_Config);
    else
        return new SSLClient(Socket::Unlock(sock), SSLClient::SERVER, m_Config);
}

SSLServer::~SSLServer()
{
}

SSLServer *SSLServer::fromSocket(int sock, const SSLConfig &ctx)
{
    return new SSLServer(sock, ctx);
}

SSLServer *SSLServer::fromSocket(TCPServer *sock, const SSLConfig &ctx)
{
    return new SSLServer(Socket::Unlock(sock), ctx);
}

SSLServer *SSLServer::Listen(int port, const SSLConfig &ctx)
    throw(SocketCantUsePort)
{
    return new SSLServer(Socket::Unlock(TCPServer::Listen(port)), ctx);
}
