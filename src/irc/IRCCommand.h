#ifndef HEADER_IRCCOMMAND_H
#define HEADER_IRCCOMMAND_H

#include <string>
#include <vector>

#include "IRCClient.h"

/**
 * A parser IRC command.
 *
 * Used internally to interpret commands from the server.
 */
class IRCCommand {

public:
    /**
     * Exception class for server command parsing.
     */
    class Invalid : public IRCError {
    public:
        Invalid(const std::string &message);
    };

    /**
     * The known command types.
     */
    enum EType {
        INVALIDPASSWD,  // 464
        BANNED,         // 465
        OTHERERROR,     // 400-599
        AWAY,           // 301 nick, message
        YOU_AWAY,       // 306
        YOU_UNAWAY,     // 305
        WHOISUSER,      // 311 nick, user, host, *, real name
        WHOISSERVER,    // 312 nick, server, server info
        WHOISOPERATOR,  // 313 nick, "is an IRC operator"
        WHOISIDLE,      // 317 nick, integer, "second idle"
        WHOISCHANNELS,  // 319 nick, [@|+]channel, ...
        ENDOFWHOIS,     // 318 nick, "End of /WHOIS list"
        CHANNELMODES,   // 324 channel, modes, mode params
        NOTOPIC,        // 331 channel, "No topic is set"
        TOPICIS,        // 332 channel, topic
        WHOREP,         // 352 channel, user, host, server, nick,
                // <H|G>[*][@|+], hopcount, realname
        ENDOFWHO,       // 315 target, "End of /WHO list"
        NAMESARE,       // 353 channel, [@|+]nick, ...
        ENDOFNAMES,     // 366 channel, "End of /NAMES list"
        BANLIST,        // 367 channel, banid
        ENDOFBANLIST,   // 368 channel, "End of channel ban list"
        MOTDSTART,      // 375 "-" <server> ...
        MOTD,           // 372 "-" ...
        ENDOFMOTD,      // 376 "End of /MOTD command"
        NOMOTD,         // 422 "MOTD File is missing"
        ISON,           // 303 nick, ...

        PRIVMSG,        // target, message
        NOTICE,         // target, message
        TOPIC,          // channel, newtopic
        JOIN,           // channel
        PART,           // channel, [reason]
        QUIT,           // reason
        NAMES,          // channel
        WHO,            // target
        MODE,
        NICK,           // newnick
        INVITE,         // nick, channel
        PING,           // somestring
        PONG,           // somestring

        UNKNOWN         // Not necessarily an error...
    };

public:
    /** The type of this command or UNKNOWN. */
    EType type;

    /**
     * The source of the command as a single string.
     *
     * You might want to use readSource(const std::string&, std::string*,
     * std::string*); note that not all fields have to be present.
     */
    std::string source;

    /**
     * The arguments of the command.
     *
     * Their meaning depends on the type of the command.
     */
    std::vector<std::string> args;

public:
    /**
     * Parse a line into an IRC command.
     */
    explicit IRCCommand(const std::string &line) throw(Invalid);

    /**
     * Simple constructor.
     *
     * Constructs an IRC command from its components.
     */
    IRCCommand(EType type_, const std::vector<std::string> &args_);

    /**
     * Simple constructor.
     *
     * Constructs an IRC command from its components.
     */
    IRCCommand(EType type_, ...);

    /**
     * Simple constructor.
     *
     * Constructs an IRC command from its components.
     */
    IRCCommand(const std::string &source, EType type_, ...);

    /**
     * Pass the source to readSource(const std::string&, std::string*,
     * std::string*).
     *
     * Provided for convenience.
     */
    std::string readSource(std::string *user, std::string *host);

public:
    /**
     * Interpret an IRC source string.
     *
     * Turns an IRC source string or mask or whatever identifies a client into
     * its components. Note that all fields don't have to be present.
     * @param user Location where to write the username or NULL.
     * @param host Location where to write the hostname or NULL.
     * @return The nickname.
     */
    static std::string readSource(const std::string &usermask,
            std::string *user, std::string *host);

};

#endif
