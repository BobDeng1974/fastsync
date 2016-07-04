/**
* server_conf.h
*
* Init Created: 2016-07-04
* Last Updated: 2016-07-04
*/
#ifndef SERVER_CONF_H_INCLUDED
#define SERVER_CONF_H_INCLUDED

#include "../mem.h"


/**
* server_conf_t is static instance in main process.
*   populated with config xml file: server.xml
*/
typedef struct server_conf_t
{
    uint32_t version;

    /**
    * authentication
    */
    char magic[6];
    int ssl_enabled;
    char pubkey[1];
    char privkey[1];

    /**
    * stash
    */
    char pathprefix[1];

    int entrydb_backend;

    union {
        /* backend: sqlite3 */
        struct {
            int maxretry;
            int interval;
        };
    };

    /**
    * tcp connection
    */
    int backlog;
    char iptable_whitelist[1];
    char iptable_blacklist[1];
    unsigned short port;
    int maxclients;
    int bufsize;
    int timeout;
    int keepinterval;
    int keepidle;
    int keepcount;
    int nodelay;

    /**
    *  autoupdate
    */
    int enable;
    char clientpkg[1];
} server_conf_t;


server_conf_t * server_conf_create(const char * cfgfile)
{
    server_conf_t * p;
    p = mem_alloc(1, sizeof(server_conf_t));
    return p;
}


void server_conf_free (server_conf_t ** srvconf)
{
    mem_free((void**) srvconf);
}


#endif /* SERVER_CONF_H_INCLUDED */
