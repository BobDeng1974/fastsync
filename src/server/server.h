/**
* server.h
*
* Init Created: 2016-07-01
* Last Updated: 2016-07-01
*/
#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include "server_conf.h"

static char VERSION[] = "0.0.1";

#define DEFAULT_PORT 4916


void print_info (const char * cfgfile, const char * log4crc, time_t start_time)
{
    printf ("\n***************************************************************");
    printf ("\n* %s-%s", FSYNC_SERVER_APP, VERSION);
    printf ("\n* Build: %s %s", __DATE__, __TIME__);
    printf ("\n* Copyright (C) 2017 pepstack.com");
    printf ("\n* Author: master@pepstack.com");
    printf ("\n***************************************************************\n");
    printf ("\n* Main process: %d", getpid());
    printf ("\n* %s/log4crc", log4crc);
    printf ("\n* Using config: %s", cfgfile);
    printf ("\n* Start time: %s\n", ctime(&start_time));
}


void set_default_config(char *cfgfile, int sizecfgfile, char *log4crc, int sizelog4crc)
{
    /* get default real client-cfg.xml path */
    int len = getpwd(cfgfile, sizecfgfile);
    if (! len) {
        fprintf(stderr, "error: %s\n\n", cfgfile);
        exit(-1);
    }
    if (len + strlen("conf/server-cfg.xml") >= sizecfgfile ||
        len + strlen("LOG4C_RCPATH=") >= sizelog4crc) {
        fprintf(stderr, "error: application path is too long: %s\n\n", cfgfile);
        exit(-1);
    }

    /* set env for LOG4C */
    snprintf(log4crc, sizelog4crc, "LOG4C_RCPATH=%sconf", cfgfile);

    if (0 != putenv(log4crc)) {
        perror(log4crc);
        exit(-1);
    }

    snprintf(cfgfile + len, sizecfgfile - len, "conf/server-cfg.xml");
}


#endif /* SERVER_H_INCLUDED */
