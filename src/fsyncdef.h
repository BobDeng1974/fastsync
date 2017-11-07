/**
* fsyncdef.h
*   common defines for server and client
*
* Init Created: 2017-01-06
* Last Updated: 2017-01-06
*/
#ifndef FSYNCDEF_H_INCLUDED
#define FSYNCDEF_H_INCLUDED

#include "mem.h"
#include "msgs.h"
#include "utils.h"
#include "sockapi.h"
#include "xmlconf.h"

#include "./common/refobject.h"

#define FSYNC_CLIENT_APP    "fastsync-client"
#define FSYNC_SERVER_APP    "fastsync-server"

#define FSYNC_HOSTNAME_LEN           60

#define FSYNC_PATHID_LEN             40
#define FSYNC_CLIENTID_LEN           FSYNC_PATHID_LEN

#define FSYNC_MAGIC_LEN               4
#define FSYNC_PATHPREFIX_LEN        255
#define FSYNC_DEFAULT_BACKLOG        16
#define FSYNC_PKGNAME_LEN            60

#define FSYNC_SERVER_MAXID          255
#define FSYNC_WPATH_MAXID          4096

#define FS__MEMBLOCK                  4
#define FS__CAPACITY(count)    (((count + FS__MEMBLOCK - 1) / FS__MEMBLOCK) * FS__MEMBLOCK)

#define CHECK_MEMORY_CAPACITY(memberList, memberType, memberCount) do { \
        int capacity = FS__CAPACITY(memberCount); \
        if (capacity == memberCount) { \
            memberList = (memberType*) realloc(memberList, (capacity + FS__MEMBLOCK) * sizeof(memberType)); \
            if (! memberList) { \
                exit(EMEMORY); \
            } \
        } \
    } while (0)


#define onerror_invalid(expr, errmsg)    do { \
        if (! (expr)) { \
            const char * em = (errmsg); \
            if (em) { \
                LOGGER_ERROR("%s", em); \
            } \
            goto onerror_exit; \
        } \
    } while (0)


#endif /* FSYNCDEF_H_INCLUDED */
