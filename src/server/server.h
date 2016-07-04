/**
* server.h
*
* Init Created: 2016-07-01
* Last Updated: 2016-07-01
*/
#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#define SERVER_APP_NAME  "fastsync-server"

#define LOGGER_COLOR_OUTPUT
#define LOGGER_CATEGORY_NAME  SERVER_APP_NAME
#include "../logger.h"

#include "../heads.h"
#include "../sockapi.h"

#ifndef MAX_FILENAME_LEN
#define MAX_FILENAME_LEN 255
#endif

#define SUCCESS     0
#define ERROR     (-1)

#include "server_conf.h"



#endif /* SERVER_H_INCLUDED */