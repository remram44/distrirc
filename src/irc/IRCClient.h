#ifndef HEADER_IRCCLIENT_H
#define HEADER_IRCCLIENT_H

#include <exception>
#include <string>

#include "sockets/Socket.h"
#include "common/ReferenceCounted.h"
#include "LineConnection.h"

class User;
class ChannelUser;
class Channel;

/**
 * Base class for exceptions thrown by IRCClient.
 */
class IRCError : public std::exception {

private:
    const std::string m_sMessage;

public:
    IRCError(const std::string &message);
    ~IRCError() throw();
    const char *what() const throw();

};


/*==============================================================================
 * Interfaces.
 */

/**
 * Callback for a WHOIS query on a user.
 */
class UserWhoisObserver {

public:
    /** Called when the WHOIS information is received back from the server. */
    virtual void whoisAvailable(User *user, const std::string &whois) = 0;

};

/**
 * Callback for channel changes.
 */
class ChannelObserver {

public:
    /**
     * Called when a new user joins the channel.
     *
     * Note that the modes of the ChannelUser are probably empty, as they will
     * be set by services (or a bot/user) in subsequent commands.
     */
    virtual void userJoinedChannel(Channel *channel, ChannelUser user) = 0;
    /**
     * Called when a user leaves the channel.
     *
     * 'reason' may be blank if the user sent no parameter to the 'PART'
     * command.
     * This is different from userQuitted(), called when a user disconnects
     * from the whole server.
     */
    virtual void userLeftChannel(Channel *channel, User *user,
            const std::string &reason) = 0;
    /**
     * Called when a user leaves the channel because he disconnected.
     *
     * 'reason' should not be blank.
     * This is different from userLeftChannel(), called when a user leaves a
     * specific channel only (still connected to the server).
     * Note that you WILL receive one of these events for each channel you were
     * both on.
     */
    virtual void userQuitted(Channel *channel, User *user,
            const std::string &reason) = 0;
    /** Called when the topic of a channel is changed. */
    virtual void topicChanged(Channel *channel, User *user,
            const std::string &new_topic) = 0;
    /**
     * Called when a user speaks on the channel.
     *
     * This only concerns the PRIVMSG command, and actions are filtered (passed
     * through the action() method instead).
     */
    virtual void message(Channel *channel, User *user,
            const std::string &msg) = 0;
    /**
     * Called when a user sends an action message to the channel.
     *
     * Action messages are specific PRIVMSGs that should be appended to the
     * nick of the user, commonly sent by using the /me client command.
     */
    virtual void action(Channel *channel, User *user,
            const std::string &action) = 0;
    /** Called when a user sends a NOTICE to the channel. */
    virtual void notice(Channel *channel, User *user,
            const std::string &msg) = 0;

};


/*==============================================================================
 * IRC objects.
 */

/**
 * Another user connected to the network.
 */
class User : public ReferenceCounted {

public:
    /**
     * Returns the nickname of the user.
     *
     * The nickname is unique on the network at a given time, but may be
     * changed.
     */
    inline std::string getNick() const;
    /**
     * Returns the username of the user.
     *
     * The username is not required to be unique, but does not change during a
     * single session.
     */
    inline std::string getUser() const;
    /**
     * Returns the host of the user.
     *
     * The host is typically the IP address or domain name associated in
     * reverse with it, as resolved by the server. Most server will anonymize
     * it to prevent access to IP addresses. A user's host may change during a
     * session.
     */
    inline std::string getHost() const;
    /**
     * Returns the realname of the user.
     *
     * The realname is a character string provided upon connecting, typically
     * used to give a hint of the real identity of a user. As it is visible to
     * everyone, most users won't put their full names there; in fact, it is
     * common to see the client's default value, eg "SomeClient user".
     * It does not change during a single session.
     */
    inline std::string getRealname() const;
    /**
     * Queries detailled user information from the server.
     *
     * As this will have to wait for the server to return the information, you
     * have to provide an observer to be notified when the information is
     * known.
     */
    void asyncWhois(UserWhoisObserver *observer);
    /** Send a private message to this user. */
    void sendMessage(const std::string &msg);
    /**
     * Send an action to this user in private.
     *
     * Action messages are specific PRIVMSGs that should be appended to the
     * nick of the user, commonly sent by using the /me client command.
     */
    void sendAction(const std::string &action);
    /**
     * Send a NOTICE to this user.
     *
     * NOTICEs are special messages that typically don't appear in a new
     * window; if you want to chat in private, PRIVMSG is what you want (use
     * the sendMessage() method).
     */
    void sendNotice(const std::string &notice);

};

/**
 * A user on a channel.
 *
 * It is simply a User plus his modes on a channel.
 */
class ChannelUser {

public:
    inline User *getUser() const;
    // TODO : modes

};

/**
 * A channel.
 */
class Channel {

public:
    /** Returns the name of the channel, with the beginning '#'. */
    inline std::string getName() const;
    /**
     * Returns the current topic of this channel.
     *
     * The topic of a channel may be changed.
     */
    inline std::string getTopic() const;
    /**
     * Leave the channel.
     *
     * The given reason string is displayed to the other users of the channel.
     */
    void leave(const std::string &reason);
    /** Send a message on this channel. */
    void sendMessage(const std::string &msg);
    /**
     * Send an action to this channel.
     *
     * Action messages are specific PRIVMSGs that should be appended to the
     * nick of the user, commonly sent by using the /me client command.
     */
    void sendAction(const std::string &msg);
    /**
     * Send a NOTICE to this channel.
     *
     * NOTICEs are special messages that are not commonly employed on channels.
     */
    void sendNotice(const std::string &msg);
    /** Change the topic of the channel. */
    void setTopic(const std::string &new_topic);
    /**
     * Add an observer to this channel.
     *
     * Implementing an observer is the only way to be notified when something
     * happens on this channel.
     */
    void addChannelObserver(ChannelObserver *observer);
    /**
     * Retrieve information on a specific user on this channel.
     *
     * @return ChannelUser() if the user is not on this channel.
     */
    ChannelUser getUser(User *user) const;
    /**
     * Retrieve information on a specific user on this channel.
     *
     * @return ChannelUser() if no user with this nick is on this channel.
     */
    ChannelUser getUser(const std::string &nick) const;

};


/*==============================================================================
 * The IRC client we can connect with.
 */

/**
 * The client.
 *
 * This is the main class of this module. Create an IRCClient to connect to an
 * IRC server and do something with the protocol.
 */
class IRCClient : public Waitable {

private:
    LineConnection *m_pConnection;

public:
    /**
     * Create a client using the given network stream.
     *
     * Typically, this stream should be a TCP connection to port 6667.
     */
    IRCClient(NetStream *stream);
    /**
     * Create a client using a LineConnection.
     */
    IRCClient(LineConnection *connection);

    void RegisterSockets(SocketSetRegistrar *registrar);

public:
    /**
     * Get a specific user on the network.
     *
     * A new User instance might get created; it may not correspond to an
     * actual user on the network (we just can't know).
     * @return NULL if the nickname is invalid.
     */
    User *getUser(const std::string &nick) const;

protected:
    /**
     * Called when a new channel is joined.
     */
    virtual void newChannel(Channel *channel) = 0;
    /**
     * Called when the connection to the server has been lost or terminated
     * with an error message.
     */
    virtual void connectionLost(const std::string &quitMsg) = 0;

};

#endif
