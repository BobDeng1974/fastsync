/**
 * client_conf.h
 *
 * Author:
 *     master@pepstack.com
 *
 * 2017-01-06: init created
 * 2017-02-01: last updated
 */
#include "client_conf.h"


static int mxml_read_configuration(mxml_node_t * node, void * param)
{
    client_conf_t * client = (client_conf_t *) param;

    const char * text;
    mxml_node_t * subnode;

    int ret;
    char unit[20];

    float second_ratio = 1.0f;
    float byte_ratio = 1.0f;

    MxmlNodeGetStringAttr(node, "timeunit", unit, sizeof(unit));
    ret = parse_second_ratio(unit, &second_ratio);
    onerror_invalid(ret, "error timeunit");

    MxmlNodeGetStringAttr(node, "sizeunit", unit, sizeof(unit));
    ret = parse_byte_ratio(unit, &byte_ratio);
    onerror_invalid(ret, "error sizeunit");

    /* clientid */
    subnode = mxmlFindElement(node, node, "clientid", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: clientid");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: clientid");
    snprintf(client->clientid, sizeof(client->clientid), "%s", text);

    MxmlNodeGetIntegerAttr(subnode, "regen", &client->regen);

    /* threads */
    subnode = mxmlFindElement(node, node, "threads", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: threads");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: threads");

    ret = atoi(text);
    onerror_invalid((ret >= CLIENT_THREADS_MIN && ret <= CLIENT_THREADS_MAX), "exceed scope: threads");
    client->threads = ret;

    /* queues */
    subnode = mxmlFindElement(node, node, "queues", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: queues");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: queues");

    ret = atoi(text);
    onerror_invalid((ret >= CLIENT_QUEUES_MIN && ret <= CLIENT_QUEUES_MAX), "exceed scope: queues");
    client->queues = ret;

    /* locktimeo */
    subnode = mxmlFindElement(node, node, "locktimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: locktimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: locktimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= CLIENT_LOCKTIMO_MIN && ret <= CLIENT_LOCKTIMO_MAX), "exceed scope: locktimeo");
    client->locktimeo = ret;

    /* bufsize */
    subnode = mxmlFindElement(node, node, "bufsize", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: bufsize");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: bufsize");

    ret = (int) (atoi(text) * byte_ratio);
    onerror_invalid((ret == 8192 || ret == 4096 || ret == 16384), "exceed scope: bufsize");
    client->bufsize = ret;

    /* sendfile */
    subnode = mxmlFindElement(node, node, "sendfile", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: sendfile");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: sendfile");

    ret = parse_bool_asint(text, &client->sendfile);
    onerror_invalid(ret, "error sendfile");

    /* sweepinterval */
    subnode = mxmlFindElement(node, node, "sweepinterval", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: sweepinterval");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: sweepinterval");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= CLIENT_SWEEPINT_MIN && ret <= CLIENT_SWEEPINT_MAX), "exceed scope: sweepinterval");
    client->sweepinterval = ret;

    return TRUE;
onerror_exit:
    return FALSE;
}


static int mxml_read_watch_path(mxml_node_t * node, void * conf)
{
    const char * text;
    mxml_node_t * subnode;

    int ret;
    char buf[512];

    float second_ratio = 1.0f;

    watch_path_t * wp = 0;

    client_conf_t * client = (client_conf_t *) conf;

    if (client->wp_count >= FSYNC_WPATH_MAXID) {
        onerror_invalid(0, "too many watch paths");
    }

    /* id, timeunit, prefix */
    ret = MxmlNodeGetStringAttr(node, "id", buf, FSYNC_PATHID_LEN + 1);
    onerror_invalid(ret, "error id");

    strlwr(buf);
    if (client_find_watch_path(client, buf)) {
        onerror_invalid(0, "duplicated watch path id");
    }

    wp = watch_path_create(buf);
    onerror_invalid(wp, "out of memory");

    MxmlNodeGetStringAttr(node, "timeunit", buf, 20);
    ret = parse_second_ratio(buf, &second_ratio);
    onerror_invalid(ret, "error timeunit");

    ret = MxmlNodeGetStringAttr(node, "prefix", wp->pathprefix, sizeof(wp->pathprefix));
    onerror_invalid(ret, "error prefix");

    /* synctimeo */
    subnode = mxmlFindElement(node, node, "synctimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: synctimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: synctimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= CLIENT_SYNCTIMO_MIN && ret <= CLIENT_SYNCTIMO_MAX), "exceed scope: synctimeo");
    wp->synctimeo = ret;

    /* zombietimeo */
    subnode = mxmlFindElement(node, node, "zombietimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "not element: zombietimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: zombietimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= CLIENT_ZOMBIETIMO_MIN && ret <= CLIENT_ZOMBIETIMO_MAX), "exceed scope: zombietimeo");
    wp->zombietimeo = ret;

    /* path-pattern */
    ret = MxmlNodeListChildNodes(node, "path-pattern", mxml_read_path_pattern, wp);
    onerror_invalid(ret, "mxml_read_path_pattern failed");

    /* file-pattern */
    ret = MxmlNodeListChildNodes(node, "file-pattern", mxml_read_file_pattern, wp);
    onerror_invalid(ret, "mxml_read_file_pattern failed");

    /* server-list */
    subnode = mxmlFindElement(node, node, "server-list", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: server-list");

    ret = MxmlNodeListChildNodes(subnode, "server", mxml_read_path_server, wp);
    onerror_invalid(ret, "mxml_read_server failed");

    /* add watch path into client */
    client_attach_watch_path(client, wp);

    return TRUE;

onerror_exit:
    watch_path_release(&wp);

    return FALSE;
}


static int mxml_read_server_authentication(mxml_node_t * node, server_conn_t *server)
{
    int ret;

    const char * text;
    mxml_node_t * subnode;

    subnode = mxmlFindElement(node, node, "magic", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: magic");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: magic");
    ret = snprintf(server->magic, sizeof(server->magic), "%s", text);
    if (FSYNC_MAGIC_LEN != ret) {
        onerror_invalid(0, "bad magic");
    }

    /* authentication */
    subnode = mxmlFindElement(node, node, "ssl", 0, 0, MXML_DESCEND);
    if (subnode) {
        MxmlNodeGetIntegerAttr(subnode, "enabled", &server->ssl_enabled);
        if (server->ssl_enabled) {
            ret = mxml_read_server_ssl_keys(subnode, server);
            onerror_invalid(ret, "mxml_read_server_ssl_keys failed");
        }
    }

    return TRUE;

onerror_exit:
    return FALSE;
}


static int mxml_read_fastsync_server(mxml_node_t * node, void * conf)
{
    const char * text;
    mxml_node_t * subnode;

    int ret;
    char buf[512];

    float second_ratio = 1.0f;

    server_conn_t * server = 0;

    client_conf_t * client = (client_conf_t *) conf;

    if (client->server_count >= FSYNC_SERVER_MAXID) {
        onerror_invalid(0, "too many servers");
    }

    /* id, timeunit, prefix */
    ret = MxmlNodeGetStringAttr(node, "id", buf, FSYNC_PATHID_LEN + 1);
    onerror_invalid(ret, "error id");

    strlwr(buf);
    if (client_find_server(client, buf)) {
        onerror_invalid(0, "duplicated fastsync server");
    }

    server = server_conn_create(buf);
    onerror_invalid(server, "out of memory");

    MxmlNodeGetStringAttr(node, "timeunit", buf, 20);
    ret = parse_second_ratio(buf, &second_ratio);
    onerror_invalid(ret, "error timeunit");

    /* host */
    subnode = mxmlFindElement(node, node, "host", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: host");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: host");

    ret = snprintf(server->host, sizeof(server->host), "%s", text);
    onerror_invalid((ret >= 8 && ret < sizeof(server->host)), "error host");

    /* port */
    subnode = mxmlFindElement(node, node, "port", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: port");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: port");

    ret = atoi(text);
    onerror_invalid((ret >= 1024 && ret < 65536), "error port");
    server->port = ret;

    /* socktimeo */
    subnode = mxmlFindElement(node, node, "socktimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: socktimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: socktimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= SEC_SOCKTIMEO_MIN && ret <= SEC_SOCKTIMEO_MAX), "error socktimeo");
    server->socktimeo = ret;

    /* recvtimeo */
    subnode = mxmlFindElement(node, node, "recvtimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: recvtimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: recvtimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= SEC_SOCKTIMEO_MIN && ret <= SEC_SOCKTIMEO_MAX), "error recvtimeo");
    server->recvtimeo = ret;

    /* sendtimeo */
    subnode = mxmlFindElement(node, node, "sendtimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: sendtimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: sendtimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= SEC_SOCKTIMEO_MIN && ret <= SEC_SOCKTIMEO_MAX), "error sendtimeo");
    server->sendtimeo = ret;

    /* checktimeo */
    subnode = mxmlFindElement(node, node, "checktimeo", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: checktimeo");

    text = mxmlGetText(subnode, 0);
    onerror_invalid(text, "bad element: checktimeo");

    ret = (int) (atoi(text) * second_ratio);
    onerror_invalid((ret >= SEC_CHECKTIMEO_MIN && ret <= SEC_CHECKTIMEO_MAX), "error checktimeo");
    server->checktimeo = ret;

    /* authentication */
    subnode = mxmlFindElement(node, node, "authentication", 0, 0, MXML_DESCEND);
    onerror_invalid(subnode, "element not found: authentication");

    ret = mxml_read_server_authentication(subnode, server);
    onerror_invalid(ret, "mxml_read_server_authentication failed");

    /* add server into client */
    client_attach_server(client, server);

    return TRUE;

onerror_exit:
    server_conn_release(&server);
    return FALSE;
}


int client_create(const char * cfgfile, client_conf_t **ppOut)
{
    int ret;

    client_conf_t * p = 0;
    mxml_node_t *tree = 0;
    FILE *fp = 0;

    *ppOut = 0;

    do {
        mxml_node_t *root;
        mxml_node_t *node;

        fp = fopen(cfgfile, "r");
        onerror_invalid(fp, strerror(errno));

        tree = mxmlLoadFile(0, fp, MXML_TEXT_CALLBACK);
        onerror_invalid(tree, "mxmlLoadFile failed");

        root = mxmlFindElement(tree, tree, "fastsync-client", 0, 0, MXML_DESCEND);
        onerror_invalid(root, "not element: fastsync-client");

        if (safe_stricmp(mxmlElementGetAttr(root, "xmlns"), "http://pepstack.com")) {
            onerror_invalid(0, "xmlns not matched: http://pepstack.com");
            goto onerror_exit;
        }

        p = (client_conf_t *) mem_alloc(1, sizeof(client_conf_t));
        onerror_invalid(p, "out of memory");

        MxmlNodeGetStringAttr(root, "ver", p->ver, sizeof(p->ver));

        /* configuration */
        node = mxmlFindElement(root, root, "configuration", 0, 0, MXML_DESCEND);
        onerror_invalid(node, "not element: configuration");

        ret = mxml_read_configuration(node, p);
        onerror_invalid(ret, "mxml_read_configuration failed");

        /* watch-path-list */
        node = mxmlFindElement(root, root, "watch-path-list", 0, 0, MXML_DESCEND);
        onerror_invalid(node, "not element: watch-path-list");

        ret = MxmlNodeListChildNodes(node, "watch-path", mxml_read_watch_path, p);
        onerror_invalid(ret, "mxml_read_watch_path failed");

        /* fastsync-server-list */
        node = mxmlFindElement(root, root, "fastsync-server-list", 0, 0, MXML_DESCEND);
        onerror_invalid(node, "not element: fastsync-server-list");

        ret = MxmlNodeListChildNodes(node, "fastsync-server", mxml_read_fastsync_server, p);
        onerror_invalid(ret, "mxml_read_fastsync_server failed");
    } while(0);

    /* load config xml success, print it */
    client_print(p);

    LOGGER_INFO("pthread_cond_init");
    if (0 != pthread_cond_init(&p->condition, 0)) {
        LOGGER_ERROR("pthread_cond_init() failed: %s", strerror(errno));
        goto onerror_exit;
    }

    LOGGER_INFO("inotify_init");
    p->infd = inotify_init();
    if (p->infd == -1) {
        LOGGER_ERROR("inotify_init() failed: %s", strerror(errno));
        pthread_cond_destroy(&p->condition);
        goto onerror_exit;
    }

    /* create per thread data */
    p->thread_args = (void **) mem_alloc(p->threads, sizeof(void*));

    for (ret = 0; ret < p->threads; ++ret) {
        int sid, sockfd;

        perthread_data perdata = (perthread_data) mem_alloc(1, sizeof(perthread_data_t) + sizeof(char)*p->bufsize);
        perdata->bufsize = p->bufsize;

        for (sid = 0; sid < p->server_count; ++sid) {
            server_conn server = p->server_list[sid];

            LOGGER_INFO("connect to server <%s:%d>, timeout=%d", server->host, server->port, server->socktimeo);

            sockfd = opensocket(server->host, server->port, server->socktimeo);
            if (! sockfd || sockfd == ERROR_SOCKET) {
                if (errno == EINPROGRESS) {
                    LOGGER_ERROR("connect to server <%s:%d> timeout: %s", server->host, (int) server->port, strerror(EINPROGRESS));
                } else {
                    LOGGER_ERROR("connect to server <%s:%d> failed: %s", server->host, (int) server->port, strerror(errno));
                }
            } else {
                LOGGER_INFO("connect to server <%s:%d> success", server->host, (int) server->port);

                close(sockfd);
            }

            //TODO:
            // perdata->sessions[sid] = server_conn_open_session (p->servs[sid], p, & sockfd);
            // perdata->sockfds[sid] = sockfd;
        }

        p->thread_args[ret] = (void*) perdata;
    }

    LOGGER_INFO("threadpool_create");
    p->pool = threadpool_create(p->threads, p->queues, p->thread_args, 0);

    mxmlDelete(tree);
    fclose(fp);

    if (RefObjectInit(p) == 0) {
        *ppOut = p;
        return SUCCESS;
    } else {
        LOGGER_INFO("RefObjectInit failed");
        exit(-1);
    }

onerror_exit:

    if (p) {
        mem_free((void**) &p);
    }

    if (tree) {
        mxmlDelete(tree);
    }

    if (fp) {
        fclose(fp);
    }

    return ERROR;
}


static void finalFreeClient (void *pv)
{
    int i, infd;

    client_conf_t * client = (client_conf_t *) pv;

    infd = client->infd;
    client->infd = -1;

    LOGGER_TRACE("pthread_cond_destroy");
    pthread_cond_destroy(&client->condition);

    /* free thread pool */
    if (client->pool) {
        threadpool_destroy(client->pool, 0);
        client->pool = 0;
        LOGGER_INFO("threadpool_destroy");
    }

    for (i = 0; i < client->wp_count; ++i) {
        watch_path_t *wp = client->wp_list[i];
        watch_path_release(&wp);
    }

    /* free thread arguments */
    if (client->thread_args) {
        for (i = 0; i < client->threads; ++i) {
            perthread_data perdata = (perthread_data) client->thread_args[i];

            if (perdata) {
                int sid;
                client->thread_args[i] = 0;

                for (sid = 0; sid < client->server_count; ++sid) {
                    int sockfd = perdata->sockfds[sid];

                    perdata->sockfds[sid] = ERROR_SOCKET;

                    if (sockfd && sockfd != ERROR_SOCKET) {
                        close(sockfd);
                    }
                }

                free(perdata);
            }
        }

        free(client->thread_args);
        client->thread_args = 0;
    }

    for (i = 0; i < client->server_count; ++i) {
        server_conn_t *scon = client->server_list[i];
        server_conn_release(&scon);
    }

    free(client);

    LOGGER_TRACE("close inotify");
    close(infd);

    LOGGER_TRACE("~client");
}


extern void client_release (client_conf_t ** client)
{
    RefObjectRelease((void**) client, finalFreeClient);
}


extern void client_print (const client_conf_t * client)
{
    int i;

    LOGGER_INFO("ver=%s", client->ver);

    LOGGER_INFO("clientid=%s", client->clientid);
    LOGGER_INFO("regen=%d", client->regen);

    LOGGER_INFO("threads=%d", client->threads);
    LOGGER_INFO("queues=%d", client->queues);

    LOGGER_INFO("locktimeo=%d", client->locktimeo);
    LOGGER_INFO("bufsize=%d", client->bufsize);
    LOGGER_INFO("sendfile=%d", client->sendfile);
    LOGGER_INFO("sweepinterval=%d", client->sweepinterval);

    LOGGER_INFO("watch_path_list(%d):", client->wp_count);
    for (i = 0;  i < client->wp_count; ++i) {
        watch_path_print(client->wp_list[i]);
    }

    LOGGER_INFO("server_list(%d):", client->server_count);
    for (i = 0;  i < client->server_count; ++i) {
        server_conn_print(client->server_list[i]);
    }
}


watch_path_t * client_find_watch_path(client_conf_t *client, const char *pathid)
{
    int i;
    watch_path_t * wp;

    for (i = 0; i < client->wp_count; ++i) {
        wp = client->wp_list[i];

        if (! strncmp(wp->pathid, pathid, sizeof(wp->pathid))) {
            return wp;
        }
    }

    return 0;
}


void client_attach_watch_path(client_conf_t *client, watch_path_t *wp)
{
    CHECK_MEMORY_CAPACITY(client->wp_list, watch_path, client->wp_count);
    client->wp_list[client->wp_count++] = wp;
}


server_conn client_find_server(client_conf_t *client, const char *sid)
{
    int i;
    server_conn  s;

    for (i = 0; i < client->server_count; ++i) {
        s = client->server_list[i];

        if (! strncmp(s->sid, sid, sizeof(s->sid))) {
            return s;
        }
    }

    return 0;
}


void client_attach_server(client_conf_t *client, server_conn server)
{
    if (client->server_count < sizeof(client->server_list)/sizeof(client->server_list[0])) {
        client->server_list[client->server_count++] = server;
    }
}
