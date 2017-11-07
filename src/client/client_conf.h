/**
 * client_conf.h
 *
 * Author:
 *     master@pepstack.com
 *
 * 2017-01-06: init created
 * 2017-01-06: last updated
 *
 */
#ifndef CLIENT_CONF_H_INCLUDED
#define CLIENT_CONF_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#include "watch_path.h"
#include "server_conn.h"


#define CLIENT_THREADS_MIN       2
#define CLIENT_THREADS_MAX       32

#define CLIENT_QUEUES_MIN        32
#define CLIENT_QUEUES_MAX        4096

#define CLIENT_SWEEPINT_MIN      6
#define CLIENT_SWEEPINT_MAX      600

#define CLIENT_LOCKTIMO_MIN      10
#define CLIENT_LOCKTIMO_MAX      (86400*30)

#define CLIENT_SYNCTIMO_MIN      10
#define CLIENT_SYNCTIMO_MAX      100000

#define CLIENT_ZOMBIETIMO_MIN    3600
#define CLIENT_ZOMBIETIMO_MAX    86400

#define SEC_SOCKTIMEO_MIN        1
#define SEC_SOCKTIMEO_MAX        30

#define SEC_CHECKTIMEO_MIN       3
#define SEC_CHECKTIMEO_MAX       300



typedef struct
{
    uint32_t sessions[FSYNC_SERVER_MAXID + 1];
    int      sockfds[FSYNC_SERVER_MAXID + 1];

    int      bufsize;
    char     pbuf[0];
} *perthread_data, perthread_data_t;

/**
 * client_conf_t
 */
typedef struct
{
    REFOBJECT_TYPE_HEADER();

    pthread_cond_t  condition;

    char ver[10];

    /* Specify an unique name in server for the client. */
    char clientid[FSYNC_CLIENTID_LEN + 1];
    int regen;

    /* number of threads */
    int threads;

    /* queue size per thread */
    int queues;

    /* thread lock error timeout in seconds */
    int locktimeo;

    /* buffer size to send data: 8192 */
    ushort bufsize;

    /* If using linux sendfile() to send data */
    int sendfile;

    /* Interval in seconds for sweeping paths */
    int infd;
    int sweepinterval;

    /* total watch_paths in wp_list */
    int wp_count;                /* count of watch paths */
    watch_path  *wp_list;
    watch_path   wp_cursor;    /* cursor for watch paths */

    /* total server_conns in conn_list */
    int server_count;
    server_conn  server_list[FSYNC_SERVER_MAXID + 1];

    /* thread pool */
    threadpool_t * pool;
    void        **thread_args;
}/* __attribute__( ( packed, aligned( 1 ) ) ) */ *client_conf, client_conf_t;


extern int client_create(const char * cfgfile, client_conf_t **ppOut);

extern void client_release (client_conf_t ** client);

extern void client_print (const client_conf_t * client);

extern watch_path_t * client_find_watch_path(client_conf_t *client, const char *pathid);

extern void client_attach_watch_path(client_conf_t *client, watch_path_t *wp);

extern server_conn client_find_server(client_conf_t *client, const char *sid);

extern void client_attach_server(client_conf_t *client, server_conn server);

#if defined(__cplusplus)
}
#endif

#endif /* CLIENT_CONF_H_INCLUDED */
