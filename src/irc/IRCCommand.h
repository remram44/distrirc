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
        CHANMSG,
        ENDOFMOTD,
        TOPIC,
        NAMES,
        ENDOFNAMES,
        WHO,
        ENDOFWHO,
        JOIN,
        PART,
        QUIT,
        MODE,
        NICK,
        UNKNOWN // Not necessarily an error...
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
    IRCCommand(const std::string &line) throw(Invalid);

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
     */
    static std::string readSource(const std::string &usermask,
            std::string *user, std::string *host);

};

#endif
