#ifndef CONFIG_H
#define CONFIG_H

#define _QUOTE2(b) #b
#define _QUOTE(a) _QUOTE2(a)

#define PROGRAM_NAME "DistrIRC"
#define PROGRAM_SHORT_NAME "distrirc"
#define PROGRAM_DESCRIPTION "DistrIRC - An IRC client by Remram <remirampin@gmail.com>"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define PROGRAM_VERSION _QUOTE(VERSION_MAJOR) "." _QUOTE(VERSION_MINOR)

#define SYSTEM_CONFIG_PATH "config.conf"
//#define USER_CONFIG_PATH ""

#endif
