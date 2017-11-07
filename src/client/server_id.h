/**
 * server_id.h
 *
 * Author:
 *     master@pepstack.com
 *
 * 2017-01-14: init created
 * 2017-01-14: last updated
 *
 */
#ifndef SERVER_ID_H_INCLUDED
#define SERVER_ID_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#include "filter_pattern.h"


typedef struct
{
    char sid[FSYNC_PATHID_LEN + 1];
    char prefix[FSYNC_PATHPREFIX_LEN + 1];

    filter_pattern_t file_included;
    filter_pattern_t file_excluded;
} server_id_t;


static server_id_t * server_id_create(const char * sid)
{
    server_id_t * serv = (server_id_t *) calloc(1, sizeof(server_id_t));
    if (serv) {
        int tolen = snprintf(serv->sid, sizeof(serv->sid), "%s", sid);
        if (tolen > 0 && tolen < sizeof(serv->sid)) {
            return serv;
        }
        free(serv);
    }
    return 0;
}


static void server_id_free(server_id_t **pserv)
{
    server_id_t *serv = *pserv;
    if (serv) {
        *pserv = 0;

        filter_pattern_free(& serv->file_included);
        filter_pattern_free(& serv->file_excluded);

        free(serv);
    }
}


static void server_id_print(server_id_t *serv)
{
    LOGGER_INFO("sid=%s", serv->sid);
    LOGGER_INFO("prefix=%s", serv->prefix);

    filter_pattern_print(&serv->file_included, "file_included");
    filter_pattern_print(&serv->file_excluded, "file_excluded");
}


static int servid_add_file_pattern(server_id_t *serv, const char *text, const char *type)
{
    const char * type_array[] = {"included", "excluded", 0};

    if (str_in_array(type, type_array)) {
        if (*type == 'i') {
            filter_pattern_add(&serv->file_included, text);
        } else {
            filter_pattern_add(&serv->file_excluded, text);
        }
        return TRUE;
    }

    return FALSE;
}


#if defined(__cplusplus)
}
#endif

#endif /* SERVER_ID_H_INCLUDED */
