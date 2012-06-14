#include "IRCCommand.h"

IRCCommand::Invalid::Invalid(const std::string &message)
  : IRCError(message)
{
}

IRCCommand::IRCCommand(const std::string &line) throw(Invalid)
{
    size_t pos = 0;
    const size_t end = line.size();

    // Read the source
    {
        pos = line.find(' ');
        if(pos == std::string::npos)
            throw Invalid("Line contains a single word");
        if(line[0] == ':')
            source = line.substr(1, pos - 1);
        else
            source = line.substr(0, pos);
        pos++;
    }

    // TODO : validate source

    // Read and recognize the command
    {
        size_t cmd_end = line.find(' ', pos);
        if(cmd_end == std::string::npos)
            cmd_end = end;
        std::string cmd = line.substr(pos, cmd_end - pos);
        if(cmd == "PRIVMSG")
            type = IRCCommand::CHANMSG;
        else if(cmd == "376" || cmd == "422")
            type = IRCCommand::ENDOFMOTD;
        else if(cmd == "332")
            type = IRCCommand::TOPIC;
        else if(cmd == "353")
            type = IRCCommand::NAMES;
        else if(cmd == "366")
            type = IRCCommand::ENDOFNAMES;
        else if(cmd == "352")
            type = IRCCommand::WHO;
        else if(cmd == "315")
            type = IRCCommand::ENDOFWHO;
        else if(cmd == "JOIN")
            type = IRCCommand::JOIN;
        else if(cmd == "PART")
            type = IRCCommand::PART;
        else if(cmd == "QUIT")
            type = IRCCommand::QUIT;
        else if(cmd == "MODE")
            type = IRCCommand::MODE;
        else if(cmd == "NICK")
            type = IRCCommand::NICK;
        else
            type = IRCCommand::UNKNOWN;

        pos = cmd_end + 1;
    }

    // On lit les paramètres
    while(pos < end)
    {
        if(line[pos] != ':' || type == IRCCommand::WHO)
        {
            if(line[pos] == ':') // Happens with the WHO command
                // hopcount is prefixed with ':' but is NOT the last command,
                // realname follows
                pos++;
            size_t param_end = line.find(' ', pos);
            if(param_end == std::string::npos)
                param_end = end;
            args.push_back(line.substr(pos, param_end - pos));
            pos = param_end;
            if(pos != end)
                pos++;
        }
        else
        {
            pos++;
            args.push_back(line.substr(pos, end - pos));
            break;
        }
    }
}

std::string IRCCommand::readSource(std::string *user, std::string *host)
{
    return readSource(source, user, host);
}

// :nick!user@host
// :nick!~user@host
// :user@host
// :host
std::string IRCCommand::readSource(const std::string &usermask,
        std::string *user, std::string *host)
{
    size_t begin = 0;
    if(usermask[0] == ':')
        begin = 1;
    size_t end = usermask.find('!');
    if(end != std::string::npos)
    {
        if(user != NULL || host != NULL)
        {
            size_t arob = usermask.find('@');
            if(arob != std::string::npos)
            {
                if(user != NULL) *user = usermask.substr(end+1, arob-end-1);
                if(host != NULL) *host = usermask.substr(arob+1);
            }
            else
            {
                // Missing fields!
                if(user != NULL) *user = "";
                if(host != NULL) *host = "";
            }
        }
        return usermask.substr(begin, end - begin);
    }
    else
    {
        if(user != NULL || host != NULL)
        {
            size_t arob = usermask.find('@');
            if(arob != std::string::npos)
            {
                // Really shouldn't happen
                if(user != NULL) *user = usermask.substr(begin, arob - begin);
                if(host != NULL) *host = usermask.substr(arob+1);
            }
            else
            {
                if(user != NULL) *user = "";
                if(host != NULL) *host = usermask.substr(begin);
            }
        }
        return "";
    }
}
