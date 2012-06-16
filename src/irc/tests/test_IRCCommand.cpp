#include <cppunit/extensions/HelperMacros.h>

#include "IRCCommand.h"

#include <iostream>

//#define _DEBUG_TEST_SESSION

struct SessionLine {
    const char *const string;
    IRCCommand command;
};

SessionLine session[] = {
    {":irc.inp-net.rezosup.org NOTICE AUTH :*** Looking up your hostname...",
     IRCCommand(IRCCommand::NOTICE, "AUTH", "*** Looking up your hostname...", NULL)},
    {":irc.inp-net.rezosup.org NOTICE AUTH :*** Found your hostname, cached",
     IRCCommand(IRCCommand::NOTICE, "AUTH", "*** Found your hostname, cached", NULL)},
    {":irc.inp-net.rezosup.org NOTICE AUTH :*** Checking Ident",
     IRCCommand(IRCCommand::NOTICE, "AUTH", "*** Checking Ident", NULL)},
    {"USER distrirc distrirc localhost :Remram",    // Sent by the client
     IRCCommand(IRCCommand::UNKNOWN, "distrirc", "distrirc", "localhost", "Remram", NULL)},
    {"NICK Test",                                   // Sent by the client
     IRCCommand(IRCCommand::NICK, "Test", NULL)},
    {":irc.inp-net.rezosup.org 001 Test :Welcome to the RezoSup IRC Network Test!distrirc@ool-18ba5d00.dyn.optonline.net",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "Welcome to the RezoSup IRC Network Test!distrirc@ool-18ba5d00.dyn.optonline.net", NULL)},
    {":irc.inp-net.rezosup.org 002 Test :Your host is irc.inp-net.rezosup.org, running version solid-ircd-3.4.8stable+rz2e-cho7",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "Your host is irc.inp-net.rezosup.org, running version solid-ircd-3.4.8stable+rz2e-cho7", NULL)},
    {"NOTICE Test :*** Your host is irc.inp-net.rezosup.org, running version solid-ircd-3.4.8stable+rz2e-cho7",
     IRCCommand(IRCCommand::NOTICE, "Test", "*** Your host is irc.inp-net.rezosup.org, running version solid-ircd-3.4.8stable+rz2e-cho7", NULL)},
    {":irc.inp-net.rezosup.org 003 Test :This server was created mercredi 15 septembre (UTC+0200) at 2010, 20:54:42",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "This server was created mercredi 15 septembre (UTC+0200) at 2010, 20:54:42", NULL)},
    {":irc.inp-net.rezosup.org 004 Test irc.inp-net.rezosup.org solid-ircd-3.4.8stable+rz2e-cho7 aAbcCdefFghHiIjkKmnoOrRsvwxXy bceiIjklLmMnoOprRstv",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "irc.inp-net.rezosup.org", "solid-ircd-3.4.8stable+rz2e-cho7", "aAbcCdefFghHiIjkKmnoOrRsvwxXy", "bceiIjklLmMnoOprRstv", NULL)},
    {":irc.inp-net.rezosup.org 005 Test NETWORK=RezoSup MAXBANS=100 MAXCHANNELS=20 CHANNELLEN=32 KICKLEN=307 NICKLEN=30 TOPICLEN=307 MODES=6 CHANTYPES=# CHANLIMIT=#:20 PREFIX=(ohv)@%+ STATUSMSG=@+ :are available on this server",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "NETWORK=RezoSup", "MAXBANS=100", "MAXCHANNELS=20", "CHANNELLEN=32", "KICKLEN=307", "NICKLEN=30", "TOPICLEN=307", "MODES=6", "CHANTYPES=#", "CHANLIMIT=#:20", "PREFIX=(ohv)@%+", "STATUSMSG=@+", "are available on this server", NULL)},
    {":irc.inp-net.rezosup.org 005 Test CASEMAPPING=ascii WATCH=128 SILENCE=10 ELIST=cmntu EXCEPTS INVEX CHANMODES=beI,k,jl,cimMnOprRstNS MAXLIST=b:100,e:45,I:100 TARGMAX=DCCALLOW:,JOIN:,KICK:4,KILL:20,NOTICE:20,PART:,PRIVMSG:20,WHOIS:,WHOWAS: :are available on this server",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "CASEMAPPING=ascii", "WATCH=128", "SILENCE=10", "ELIST=cmntu", "EXCEPTS", "INVEX", "CHANMODES=beI,k,jl,cimMnOprRstNS", "MAXLIST=b:100,e:45,I:100", "TARGMAX=DCCALLOW:,JOIN:,KICK:4,KILL:20,NOTICE:20,PART:,PRIVMSG:20,WHOIS:,WHOWAS:", "are available on this server", NULL)},
    {":irc.inp-net.rezosup.org 251 Test :There are 9 users and 624 invisible on 18 servers",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "There are 9 users and 624 invisible on 18 servers", NULL)},
    {":irc.inp-net.rezosup.org 252 Test 26 :IRC Operators online",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "26", "IRC Operators online", NULL)},
    {":irc.inp-net.rezosup.org 254 Test 471 :channels formed",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "471", "channels formed", NULL)},
    {":irc.inp-net.rezosup.org 255 Test :I have 80 clients and 1 servers",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "I have 80 clients and 1 servers", NULL)},
    {":irc.inp-net.rezosup.org 265 Test :Current local users: 80 Max: 302",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "Current local users: 80 Max: 302", NULL)},
    {":irc.inp-net.rezosup.org 266 Test :Current global users: 633 Max: 937",
     IRCCommand(IRCCommand::UNKNOWN, "Test", "Current global users: 633 Max: 937", NULL)},
    {":irc.inp-net.rezosup.org NOTICE Test :*** Notice -- motd was last changed at 9/7/2011 20:59",
     IRCCommand(IRCCommand::NOTICE, "Test", "*** Notice -- motd was last changed at 9/7/2011 20:59", NULL)},
    {":irc.inp-net.rezosup.org 375 Test :- irc.inp-net.rezosup.org Message of the Day -",
     IRCCommand(IRCCommand::MOTDSTART, "Test", "- irc.inp-net.rezosup.org Message of the Day -", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-9/7/2011 20:59",
     IRCCommand(IRCCommand::MOTD, "Test", "-9/7/2011 20:59", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-",
     IRCCommand(IRCCommand::MOTD, "Test", "-", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-       ____                _____",
     IRCCommand(IRCCommand::MOTD, "Test", "-       ____                _____", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-      / __ \\___ ____ ____ / ___/__  ______",
     IRCCommand(IRCCommand::MOTD, "Test", "-      / __ \\___ ____ ____ / ___/__  ______", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-     / /_/ / _ Y_  // __ \\\\__ \\/ / / / __ \\",
     IRCCommand(IRCCommand::MOTD, "Test", "-     / /_/ / _ Y_  // __ \\\\__ \\/ / / / __ \\", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-    / _, _/  __// // /_/ /__/ / /_/ / /_/ /",
     IRCCommand(IRCCommand::MOTD, "Test", "-    / _, _/  __// // /_/ /__/ / /_/ / /_/ /", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-   /_/ |_|\\___//___|____/____/\\__,_/ .___/",
     IRCCommand(IRCCommand::MOTD, "Test", "-   /_/ |_|\\___//___|____/____/\\__,_/ .___/", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-                                  /_/",
     IRCCommand(IRCCommand::MOTD, "Test", "-                                  /_/", NULL)},
    {":irc.inp-net.rezosup.org 372 Test :-",
     IRCCommand(IRCCommand::MOTD, "Test", "-", NULL)},
    {":irc.inp-net.rezosup.org 376 Test :End of /MOTD command.",
     IRCCommand(IRCCommand::ENDOFMOTD, "Test", "End of /MOTD command.", NULL)},
    {":Test MODE Test :+iv",
     IRCCommand(IRCCommand::MODE, "Test", "+iv", NULL)},
    {":Global!services@services.rezosup.net NOTICE Test :[Logon News - 19 Jun 2005] Pour desactiver le masquage du nom d'hote, tapez la commande /mode votre_pseudo -v",
     IRCCommand(IRCCommand::NOTICE, "Test", "[Logon News - 19 Jun 2005] Pour desactiver le masquage du nom d'hote, tapez la commande /mode votre_pseudo -v", NULL)},
    {"JOIN #rezo",
     IRCCommand(IRCCommand::JOIN, "#rezo", NULL)},
    {":Test!distrirc@RZ-77dd3605.dyn.optonline.net JOIN :#rezo",
     IRCCommand(IRCCommand::JOIN, "#rezo", NULL)},
    {":irc.inp-net.rezosup.org 332 Test #rezo :Supélec Rézo | https://doc.rez-gif.supelec.fr | Logs IRC : https://www.rez-gif.supelec.fr/irclogs/2012/Rezosup/ | Bureau : TsCl (Prez) , Horo, Alef_Burzmali (Vprezs), eXenon (Trez), gxs (Screz)",
     IRCCommand(IRCCommand::TOPICIS, "Test", "#rezo", "Supélec Rézo | https://doc.rez-gif.supelec.fr | Logs IRC : https://www.rez-gif.supelec.fr/irclogs/2012/Rezosup/ | Bureau : TsCl (Prez) , Horo, Alef_Burzmali (Vprezs), eXenon (Trez), gxs (Screz)", NULL)},
    {":irc.inp-net.rezosup.org 333 Test #rezo Zertrin 1337808621", // FIXME : 333 is not in RFC 1459
     IRCCommand(IRCCommand::UNKNOWN, "Test", "#rezo", "Zertrin", "1337808621", NULL)},
    {":irc.inp-net.rezosup.org 353 Test @ #rezo :Test @guitou ttdx BuLi @Remram @exenon @TsCl_ @ciblout paradis @Alef_Burzmali @Zertrin K-Yo clempar _Lily_ @DaLynX TheRezoRoux Tsakagur rr4botz2 @serianox horo bbc @phyce @VnC_ Z_ kage Gagou",
     IRCCommand(IRCCommand::NAMESARE, "Test", "@", "#rezo", "Test", "@guitou", "ttdx", "BuLi", "@Remram", "@exenon", "@TsCl_", "@ciblout", "paradis", "@Alef_Burzmali", "@Zertrin", "K-Yo", "clempar", "_Lily_", "@DaLynX", "TheRezoRoux", "Tsakagur", "rr4botz2", "@serianox", "horo", "bbc", "@phyce", "@VnC_", "Z_", "kage", "Gagou", NULL)},
    {":irc.inp-net.rezosup.org 366 Test #rezo :End of /NAMES list.",
     IRCCommand(IRCCommand::ENDOFNAMES, "Test", "#rezo", "End of /NAMES list.", NULL)},
    {"NAMES #supelec",
     IRCCommand(IRCCommand::NAMES, "#supelec", NULL)},
    {":irc.inp-net.rezosup.org 353 Test = #supelec :@Electron",
     IRCCommand(IRCCommand::NAMESARE, "Test", "=", "#supelec", "@Electron", NULL)},
    {":irc.inp-net.rezosup.org 366 Test #supelec :End of /NAMES list.",
     IRCCommand(IRCCommand::ENDOFNAMES, "Test", "#supelec", "End of /NAMES list.", NULL)},
    {"WHO #rezo",
     IRCCommand(IRCCommand::WHO, "#rezo", NULL)},
    {":irc.inp-net.rezosup.org 352 Test #rezo distrirc RZ-77dd3605.dyn.optonline.net irc.inp-net.rezosup.org Test H :0 Remram",
     IRCCommand(IRCCommand::WHOREP, "Test", "#rezo", "distrirc", "RZ-77dd3605.dyn.optonline.net", "irc.inp-net.rezosup.org", "Test", "H", "0", "Remram", NULL)},
    {":irc.inp-net.rezosup.org 352 Test #rezo tscl RZ-b2fe20de.rez-gif.supelec.fr irc.supelec.rezosup.org BuLi H :3 Pierre Montagnier",
     IRCCommand(IRCCommand::WHOREP, "Test", "#rezo", "tscl", "RZ-b2fe20de.rez-gif.supelec.fr", "irc.supelec.rezosup.org", "BuLi", "H", "3", "Pierre Montagnier", NULL)},
    {":irc.inp-net.rezosup.org 352 Test #rezo Remram staff.supelec.rezosup.net irc.supelec.rezosup.org Remram H*@ :3 Remi Rampin",
     IRCCommand(IRCCommand::WHOREP, "Test", "#rezo", "Remram", "staff.supelec.rezosup.net", "irc.supelec.rezosup.org", "Remram", "H*@", "3", "Remi Rampin", NULL)},
    {":irc.inp-net.rezosup.org 352 Test #rezo zertrin RZ-c8308929.rez-gif.supelec.fr irc.u-psud.rezosup.org Zertrin G@ :3 Zertrin",
     IRCCommand(IRCCommand::WHOREP, "Test", "#rezo", "zertrin", "RZ-c8308929.rez-gif.supelec.fr", "irc.u-psud.rezosup.org", "Zertrin", "G@", "3", "Zertrin", NULL)},
    {":irc.inp-net.rezosup.org 352 Test #rezo quassel RZ-c8308929.rez-gif.supelec.fr irc.supelec.rezosup.org _Lily_ G :3 Marc Delorme",
     IRCCommand(IRCCommand::WHOREP, "Test", "#rezo", "quassel", "RZ-c8308929.rez-gif.supelec.fr", "irc.supelec.rezosup.org", "_Lily_", "G", "3", "Marc Delorme", NULL)},
    {":irc.inp-net.rezosup.org 315 Test #rezo :End of /WHO list.",
     IRCCommand(IRCCommand::ENDOFWHO, "Test", "#rezo", "End of /WHO list.", NULL)},
    {"PING :this is sparta",
     IRCCommand(IRCCommand::PING, "this is sparta", NULL)},
    {":irc.inp-net.rezosup.org PONG irc.inp-net.rezosup.org :this is sparta",
     IRCCommand(IRCCommand::PONG, "irc.inp-net.rezosup.org", "this is sparta", NULL)},
    {"QUIT :end of test",
     IRCCommand(IRCCommand::QUIT, "end of test", NULL)},
    {"ERROR :Closing Link: ool-18ba5d00.dyn.optonline.net (Quit: end of test)",
     IRCCommand(IRCCommand::UNKNOWN, "Closing Link: ool-18ba5d00.dyn.optonline.net (Quit: end of test)", NULL)}
};

class IRCCommand_test : public CppUnit::TestFixture {

public:
    void test_readSource()
    {
        std::string nick, user, host;

        nick = IRCCommand::readSource(
                "Remram[away]!distrirc@remram44.github.com",
                &user, &host);
        CPPUNIT_ASSERT(nick == "Remram[away]");
        CPPUNIT_ASSERT(user == "distrirc");
        CPPUNIT_ASSERT(host == "remram44.github.com");

        nick = IRCCommand::readSource(
                "Remr@m[away]!distrirc@remram44.github.com",
                &user, &host);
        CPPUNIT_ASSERT(nick == "Remr@m[away]");
        CPPUNIT_ASSERT(user == "distrirc");
        CPPUNIT_ASSERT(host == "remram44.github.com");

        nick = IRCCommand::readSource(
                ":distrirc@remram44.github.com",
                &user, &host);
        CPPUNIT_ASSERT(nick == "");
        CPPUNIT_ASSERT(user == "distrirc");
        CPPUNIT_ASSERT(host == "remram44.github.com");

        nick = IRCCommand::readSource(
                "remram44.github.com",
                &user, &host);
        CPPUNIT_ASSERT(nick == "");
        CPPUNIT_ASSERT(user == "");
        CPPUNIT_ASSERT(host == "remram44.github.com");
    }

    void test_commands()
    {
        size_t i;
        for(i = 0; i < sizeof(session)/sizeof(SessionLine); i++)
        {
            const IRCCommand cmd(session[i].string);
            const IRCCommand &other = session[i].command;
#ifdef _DEBUG_TEST_SESSION
            {
                std::cerr << "EType(" << cmd.type << ")";
                size_t j;
                for(j = 0; j < cmd.args.size(); j++)
                    std::cerr << " <" << cmd.args[j] << ">";
                std::cerr << "\n";
            }
#endif
            CPPUNIT_ASSERT(cmd.type == other.type);
            CPPUNIT_ASSERT(cmd.args.size() == other.args.size());
            size_t i;
            for(i = 0; i < cmd.args.size(); i++)
                CPPUNIT_ASSERT(cmd.args[i] == other.args[i]);
        }
    }

    CPPUNIT_TEST_SUITE(IRCCommand_test);
    CPPUNIT_TEST(test_readSource);
    CPPUNIT_TEST(test_commands);
    CPPUNIT_TEST_SUITE_END();

};

CPPUNIT_TEST_SUITE_REGISTRATION(IRCCommand_test);
