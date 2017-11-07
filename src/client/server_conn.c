/**
 * server_conn.c
 *
 * Author:
 *     master@pepstack.com
 *
 * 2016-07-20: init created
 * 2017-01-14: last updated
 */
#include "server_conn.h"


static void finalFreeServerConn(void * pv)
{
    server_conn sconn  = (server_conn) pv;

    LOGGER_WARN("TODO: close socket connection");

    free(sconn);

    LOGGER_TRACE("~server_conn");
}


server_conn server_conn_create(char *sid)
{
    server_conn sp = (server_conn) calloc(1, sizeof(server_conn_t));
    if (sp) {
        int tolen = snprintf(sp->sid, sizeof(sp->sid), "%s", sid);
        if (tolen > 0 && tolen < sizeof(sp->sid)) {
            RefObjectInit(sp);
            LOGGER_TRACE("server_conn");
            return sp;
        }
        free(sp);
    }
    return 0;
}


void server_conn_release(server_conn *pserv)
{
    RefObjectRelease((void**) pserv, finalFreeServerConn);
}


void server_conn_print(server_conn serv)
{
    LOGGER_INFO("sid=%s", serv->sid);
    LOGGER_INFO("host=%s", serv->host);
    LOGGER_INFO("port=%d", (int) serv->port);

    LOGGER_INFO("socktimeo=%d", (int) serv->socktimeo);
    LOGGER_INFO("recvtimeo=%d", (int) serv->recvtimeo);
    LOGGER_INFO("sendtimeo=%d", (int) serv->sendtimeo);

    LOGGER_INFO("checktimeo=%d", (int) serv->checktimeo);

    LOGGER_INFO("magic=%s", serv->magic);

    LOGGER_INFO("ssl_enabled=%d", serv->ssl_enabled);
    LOGGER_INFO("ssl_pubkey=%s", serv->ssl_pubkey);
    LOGGER_INFO("ssl_privkey=%s", serv->ssl_privkey);
}
