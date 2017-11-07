/**
 * watch_path.h
 *
 * Author:
 *     master@pepstack.com
 *
 * 2017-01-06: init created
 * 2017-01-06: last updated
 *
 */
#ifndef WATCH_PATH_H_INCLUDED
#define WATCH_PATH_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#include "server_id.h"
#include "../inotifyapi.h"

/**
 * watch_path_t
 */
typedef struct
{
    REFOBJECT_TYPE_HEADER();

    char pathid[FSYNC_PATHID_LEN + 1];
    char pathprefix[FSYNC_PATHPREFIX_LEN + 1];

    /* path-pattern */
    filter_pattern_t path_included;
    filter_pattern_t path_excluded;

    /* file-pattern */
    filter_pattern_t file_included;
    filter_pattern_t file_excluded;

    /* Sync file timeout in seconds */
    uint32_t synctimeo;

    /* Zombie file timeout in seconds */
    uint32_t zombietimeo;

    /* server id list */
    int sid_count;
    server_id_t *sid_list[FSYNC_SERVER_MAXID + 1];
}/* __attribute__( ( packed, aligned( 1 ) ) ) */ * watch_path, watch_path_t;


/**
 * watch_path_t
 */
extern watch_path_t * watch_path_create(const char *pathid);

extern void watch_path_release(watch_path_t **wp);

extern void watch_path_print(const watch_path_t *wp);

extern int watch_path_add_path_pattern(watch_path_t *wp, const char *text, const char *type);

extern int watch_path_add_file_pattern(watch_path_t *wp, const char *text, const char *type);

extern server_id_t * watch_path_find_serverid(watch_path_t *wp, const char *sid);

extern void watch_path_attach_serverid(watch_path_t *wp, server_id_t *server);


/**
 * internal functions
 */
static int mxml_read_path_pattern(mxml_node_t * node, void * wpath)
{
    const char * text;
    char type[10];

    watch_path_t * wp = (watch_path_t *) wpath;

    text = mxmlGetText(node, 0);
    onerror_invalid(text, "bad element: path_pattern");

    if (MxmlNodeGetStringAttr(node, "type", type, sizeof(type))) {
        return watch_path_add_path_pattern(wp, text, strlwr(type));
    }

onerror_exit:
    return FALSE;
}


static int mxml_read_file_pattern(mxml_node_t * node, void * wpath)
{
    const char * text;
    char type[10];

    watch_path_t * wp = (watch_path_t *) wpath;

    text = mxmlGetText(node, 0);
    onerror_invalid(text, "bad element: file_pattern");

    if (MxmlNodeGetStringAttr(node, "type", type, sizeof(type))) {
        return watch_path_add_file_pattern(wp, text, strlwr(type));
    }

onerror_exit:
    return FALSE;
}


static int server_read_file_pattern(mxml_node_t * node, void * servid)
{
    const char * text;
    char type[10];

    text = mxmlGetText(node, 0);
    onerror_invalid(text, "bad element: file_pattern");

    if (MxmlNodeGetStringAttr(node, "type", type, sizeof(type))) {
        return servid_add_file_pattern((server_id_t *)servid, text, strlwr(type));
    }

onerror_exit:
    return FALSE;
}


static int mxml_read_path_server(mxml_node_t * node, void * wpath)
{
    int ret;

    char sid[FSYNC_PATHID_LEN + 1];

    server_id_t * serv = 0;

    watch_path_t * wp = (watch_path_t *) wpath;

    if (wp->sid_count >= FSYNC_SERVER_MAXID) {
        onerror_invalid(0, "too many server id");
    }

    /* id, prefix */
    ret = MxmlNodeGetStringAttr(node, "id", sid, sizeof(sid));
    onerror_invalid(ret, "error id");
    strlwr(sid);

    if (watch_path_find_serverid(wp, sid)) {
        goto onerror_exit;
    }

    serv = server_id_create(sid);
    onerror_invalid(serv, "out of memory");

    ret = MxmlNodeGetStringAttr(node, "prefix", serv->prefix, sizeof(serv->prefix));
    onerror_invalid(ret, "error prefix");

    strlwr(serv->prefix);

    ret = MxmlNodeListChildNodes(node, "file-pattern", server_read_file_pattern, serv);
    onerror_invalid(ret, "error file-pattern");

    watch_path_attach_serverid(wp, serv);

    return TRUE;

onerror_exit:
    server_id_free(&serv);
    return FALSE;
}

#if defined(__cplusplus)
}
#endif

#endif /* WATCH_PATH_H_INCLUDED */
