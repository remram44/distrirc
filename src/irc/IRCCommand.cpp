#include "IRCCommand.h"

#include <cstdarg>

static const char *const COMMANDS[IRCCommand::UNKNOWN] = {
    "464", // INVALIDPASSWD
    "465", // BANNED
    "",    // OTHERERROR
    "301", // AWAY
    "306", // YOU_AWAY
    "305", // YOU_UNAWAY
    "311", // WHOISUSER
    "312", // WHOISSERVER
    "313", // WHOISOPERATOR
    "317", // WHOISIDLE
    "319", // WHOISCHANNELS
    "318", // ENDOFWHOIS
    "324", // CHANNELMODES
    "331", // NOTOPIC
    "332", // TOPICIS
    "333", // TOPICWHOTIME
    "352", // WHOREP
    "315", // ENDOFWHO
    "353", // NAMESARE
    "366", // ENDOFNAMES
    "367", // BANLIST
    "368", // ENDOFBANLIST
    "375", // MOTDSTART
    "372", // MOTD
    "376", // ENDOFMOTD
    "422", // NOMOTD
    "303", // ISON

    "PRIVMSG", // PRIVMSG
    "NOTICE", // NOTICE
    "TOPIC", // TOPIC
    "JOIN", // JOIN
    "PART", // PART
    "QUIT", // QUIT
    "NAMES", // NAMES
    "WHO", // WHO,
    "MODE", // MODE,
    "NICK", // NICK
    "INVITE", // INVITE
    "PING", // PING
    "PONG", // PONG
};

IRCCommand::Invalid::Invalid(const std::string &message)
  : IRCError(message)
{
}

IRCCommand::IRCCommand(const std::string &line) throw(Invalid)
{
    size_t pos = 0;
    const size_t end = line.size();

    if(end == 0)
        throw Invalid("Line is empty");

    // Read the source
    if(line[0] == ':')
    {
        pos = line.find(' ', 1);
        if(pos == std::string::npos)
            throw Invalid("Line contains doesn't contain a command");
        source = line.substr(1, pos - 1);
        pos++;
    }
    else
        source = "";

    // Read and recognize the command
    {
        size_t cmd_end = line.find(' ', pos);
        if(cmd_end == std::string::npos)
            cmd_end = end;
        std::string cmd = line.substr(pos, cmd_end - pos);
        type = UNKNOWN;
        int i;
        for(i = 0; i < UNKNOWN; i++)
            if(cmd == COMMANDS[i])
                type = (EType)i;
        if(type == UNKNOWN)
        {
            const char *ptr = cmd.c_str();
            char *endptr;
            int num = strtol(ptr, &endptr, 10);
            if( (endptr == ptr + cmd.size()) && 400 <= num && num <= 599)
                type = OTHERERROR;
        }

        pos = cmd_end + 1;
    }

    // Read the arguments
    while(pos < end)
    {
        // Various fixes for commands that use unpractical formats (ex. with
        // misplaced ':')
        bool ignore_colon = (
                type == WHOISCHANNELS ||
                type == WHOREP ||
                type == NAMESARE ||
                type == ISON);
        bool force_colon = (
                type == WHOREP &&
                args.size() == 8);
        bool colon = line[pos] == ':';
        if(colon)
            pos++;
        if( (colon && !ignore_colon) || force_colon)
        {
            args.push_back(line.substr(pos, end - pos));
            break;
        }
        else
        {
            size_t param_end = line.find(' ', pos);
            if(param_end == std::string::npos)
                param_end = end;
            args.push_back(line.substr(pos, param_end - pos));
            pos = param_end;
            if(pos != end)
                pos++;
        }
    }
}

IRCCommand::IRCCommand(EType type_, const std::vector<std::string> &args_)
  : type(type_), args(args_)
{
}

IRCCommand::IRCCommand(EType type_, ...)
  : type(type_)
{
    va_list ap;
    va_start(ap, type_);
    const char *args_;
    while((args_ = va_arg(ap, const char*)) != NULL)
        args.push_back(args_);
    va_end(ap);
}

IRCCommand::IRCCommand(const std::string &source_, EType type_, ...)
  : type(type_), source(source_)
{
    va_list ap;
    va_start(ap, type_);
    const char *args_;
    while((args_ = va_arg(ap, const char*)) != NULL)
        args.push_back(args_);
    va_end(ap);
}

std::string IRCCommand::readSource(std::string *user, std::string *host)
{
    return readSource(source, user, host);
}

// 1 [:]nick!user@host
// 2 [:]user@host
// 3 [:]host
std::string IRCCommand::readSource(const std::string &usermask,
        std::string *user, std::string *host)
{
    size_t begin = 0;
    if(usermask[0] == ':')
        begin = 1;

    size_t ar = usermask.rfind('@');

    // Case 3
    if(ar == std::string::npos)
    {
        if(user != NULL) *user = "";
        if(host != NULL) *host = usermask.substr(begin);
        return "";
    }
    // Case 1 or 2
    else
    {
        size_t ex = usermask.rfind('!', ar);

        // Case 2
        if(ex == std::string::npos)
        {
            if(user != NULL) *user = usermask.substr(begin, ar - begin);
            if(host != NULL) *host = usermask.substr(ar + 1);
            return "";
        }
        // Case 1
        else
        {
            if(user != NULL) *user = usermask.substr(ex + 1, ar - ex - 1);
            if(host != NULL) *host = usermask.substr(ar + 1);
            return usermask.substr(begin, ex - begin);
        }
    }
}
