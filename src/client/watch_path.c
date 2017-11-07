/**
 * watch_path.c
 *
 * Author:
 *     master@pepstack.com
 *
 * 2017-01-14: init created
 * 2017-01-14: last updated
 *
 */
#include "watch_path.h"


watch_path_t * watch_path_create(const char *pathid)
{
    watch_path_t * wp = (watch_path_t *) calloc(1, sizeof(watch_path_t));
    if (wp) {
        int tolen = snprintf(wp->pathid, sizeof(wp->pathid), "%s", pathid);
        if (tolen > 0 && tolen < sizeof(wp->pathid)) {
            RefObjectInit(wp);
            LOGGER_TRACE("watch_path");
            return wp;
        }
        free(wp);
    }
    return 0;
}


static void finalFreeWatchPath(void * pv)
{
    int i;
    watch_path_t *wp  = (watch_path_t *) pv;

    filter_pattern_free(& wp->path_included);
    filter_pattern_free(& wp->path_excluded);
    filter_pattern_free(& wp->file_included);
    filter_pattern_free(& wp->file_excluded);

    for (i = 0; i < wp->sid_count; ++i) {
        server_id_t *sid = wp->sid_list[i];
        server_id_free(&sid);
    }

    free(wp);

    LOGGER_TRACE("~watch_path");
}


void watch_path_release(watch_path_t **wpp)
{
    RefObjectRelease((void**) wpp, finalFreeWatchPath);
}


void watch_path_print(const watch_path_t *wp)
{
    int i;

    LOGGER_INFO("pathid=%s", wp->pathid);
    LOGGER_INFO("pathprefix=%s", wp->pathprefix);

    LOGGER_INFO("synctimeo=%d", wp->synctimeo);
    LOGGER_INFO("zombietimeo=%d", wp->zombietimeo);

    filter_pattern_print(&(wp->path_included), "path_included");
    filter_pattern_print(&(wp->path_excluded), "path_excluded");

    filter_pattern_print(&(wp->file_included), "file_included");
    filter_pattern_print(&(wp->file_excluded), "file_excluded");

    LOGGER_INFO("server_id_list(%d):", wp->sid_count);
    for (i = 0;  i < wp->sid_count; ++i) {
        server_id_print(wp->sid_list[i]);
    }
}


int watch_path_add_path_pattern(watch_path_t *wp, const char *text, const char *type)
{
    const char * type_array[] = {"included", "excluded", 0};

    if (str_in_array(type, type_array)) {
        if (*type == 'i') {
            filter_pattern_add(&wp->path_included, text);
        } else {
            filter_pattern_add(&wp->path_excluded, text);
        }
        return TRUE;
    }

    return FALSE;
}


int watch_path_add_file_pattern(watch_path_t *wp, const char *text, const char *type)
{
    const char * type_array[] = {"included", "excluded", 0};

    if (str_in_array(type, type_array)) {
        if (*type == 'i') {
            filter_pattern_add(&wp->file_included, text);
        } else {
            filter_pattern_add(&wp->file_excluded, text);
        }
        return TRUE;
    }

    return FALSE;
}


server_id_t * watch_path_find_serverid(watch_path_t *wp, const char *sid)
{
    int i;
    server_id_t * serv;

    for (i = 0; i < wp->sid_count; ++i) {
        serv = wp->sid_list[i];

        if (! strncmp(serv->sid, sid, sizeof(serv->sid))) {
            return serv;
        }
    }

    return 0;
}


void watch_path_attach_serverid(watch_path_t *wp, server_id_t *server)
{
    if (wp->sid_count < sizeof(wp->sid_list)/sizeof(wp->sid_list[0])) {
        wp->sid_list[wp->sid_count++] = server;
    }
}


