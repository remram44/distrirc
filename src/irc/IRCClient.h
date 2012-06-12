#ifndef HEADER_IRCCLIENT_H
#define HEADER_IRCCLIENT_H

#include <string>

#include "sockets/Socket.h"
#include "common/ReferenceCounted.h"
#include "LineConnection.h"

class User;
class ChannelUser;
class Channel;


/*==============================================================================
 * Interfaces.
 */

class UserWhoisObserver {

public:
    virtual void whoisAvailable(User *user, const std::string &whois) = 0;

};

class ChannelObserver {

public:
    virtual void userJoinedChannel(Channel *channel, ChannelUser *user) = 0;
    virtual void userLeftChannel(Channel *channel, User *user,
            const std::string &reason) = 0;
    virtual void topicChanged(Channel *channel, User *user,
            const std::string &new_topic) = 0;
    virtual void message(Channel *channel, User *user,
            const std::string &msg) = 0;
    virtual void action(Channel *channel, User *user,
            const std::string &action) = 0;
    virtual void notice(Channel *channel, User *user,
            const std::string &msg) = 0;

};


/*==============================================================================
 * IRC objects.
 */

class User : public ReferenceCounted {

public:
    inline std::string getNick() const;
    inline std::string getUser() const;
    inline std::string getHost() const;
    inline std::string getRealname() const;
    void asyncWhois(UserWhoisObserver *observer = NULL);
    void sendMessage(const std::string &msg);
    void sendAction(const std::string &action);
    void sendNotice(const std::string &notice);

};

class ChannelUser : public ReferenceCounted {

public:
    inline User *getUser() const;
    // TODO : modes

};

class Channel {

public:
    inline std::string getName() const;
    inline std::string getTopic() const;
    void leave(const std::string &reason);
    void sendMessage(const std::string &msg);
    void sendAction(const std::string &msg);
    void sendNotice(const std::string &msg);
    void setTopic(const std::string &new_topic);
    void addChannelObserver(ChannelObserver *observer);
    ChannelUser *getUser(User *user) const;
    ChannelUser *getUser(const std::string &nick) const;

};


/*==============================================================================
 * The IRC client we can connect with.
 */

class IRCClient : public Waitable {

private:
    LineConnection *m_pConnection;

public:
    IRCClient(NetStream *stream);
    IRCClient(LineConnection *connection);

    void RegisterSockets(SocketSetRegistrar *registrar);

public:
    User *getUser(const std::string &nick) const;

protected:
    virtual void newChannel(Channel *channel) = 0;
    virtual void connectionLost(const std::string &quitMsg) = 0;

};

#endif
