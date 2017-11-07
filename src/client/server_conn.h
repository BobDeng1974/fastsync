/**
 * server_conn.h
 *
 * 2016-07-20: init created
 * 2017-01-14: last updated
 */
#ifndef SERVER_CONN_H_INCLUDED
#define SERVER_CONN_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#include "filter_pattern.h"


typedef struct
{
    REFOBJECT_TYPE_HEADER();

    char sid[FSYNC_PATHID_LEN + 1];

    char host[FSYNC_HOSTNAME_LEN + 1];
    unsigned short port;

    int socktimeo;
    int recvtimeo;
    int sendtimeo;

    int checktimeo;

    /* authentication */
    char magic[FSYNC_MAGIC_LEN + 1];

    int ssl_enabled;
    char ssl_pubkey[MAX_FILENAME_LEN + 1];
    char ssl_privkey[MAX_FILENAME_LEN + 1];
} *server_conn, server_conn_t;


static int mxml_read_server_ssl_keys(mxml_node_t * node, server_conn_t * server)
{
    int ret;

    const char * text;
    mxml_node_t *subnode;

    /* pubkey */
    subnode = mxmlFindElement(node, node, "pubkey", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: pubkey");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: pubkey");

    ret = snprintf(server->ssl_pubkey, sizeof(server->ssl_pubkey), "%s", text);
    onerror_invalid((ret > 0 && ret < sizeof(server->ssl_pubkey)), "error pubkey");

    /* privkey */
    subnode = mxmlFindElement(node, node, "privkey", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: privkey");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: privkey");

    ret = snprintf(server->ssl_privkey, sizeof(server->ssl_privkey), "%s", text);
    onerror_invalid((ret > 0 && ret < sizeof(server->ssl_privkey)), "error privkey");

    return TRUE;

onerror_exit:
    server->ssl_enabled = 0;
    return FALSE;
}


extern server_conn server_conn_create(char *sid);

extern void server_conn_release(server_conn *pserv);

extern void server_conn_print(server_conn serv);

#if defined(__cplusplus)
}
#endif

#endif /* SERVER_CONN_H_INCLUDED */
