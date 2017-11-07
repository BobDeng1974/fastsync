/**
 * inotifyapi.h
 *
 * 2014-08-01: init created
 * zhangliang
 */
#ifndef INOTIFYAPI_H_INCLUDED
#define INOTIFYAPI_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/inotify.h>


#define USER_SWEEP_NEW  0xfff0
#define USER_SWEEP_MOD  0xfff1
#define USER_SWEEP_LOG  0xfff2


/* ignore events:
 * IN_CLOSE_NOWRITE | IN_ACCESS
 */

#define WATCH_MASK  ( \
        IN_Q_OVERFLOW | \
        IN_DELETE_SELF | \
        IN_MOVE_SELF | \
        IN_UNMOUNT | \
        IN_ISDIR | \
        IN_CLOSE_WRITE | \
        IN_DELETE | \
        IN_MOVED_FROM | \
        IN_MOVED_TO | \
        IN_MODIFY | \
        IN_CREATE \
    )

static char EVT_IN_Q_OVERFLOW[] = "Q_OVERFLOW";
static char EVT_IN_DELETE_SELF[] = "DELETE_SELF";
static char EVT_IN_MOVE_SELF[] = "MOVE_SELF";
static char EVT_IN_UNMOUNT[] = "UNMOUNT";
static char EVT_IN_ISDIR[] = "ISDIR";
static char EVT_IN_CLOSE_WRITE[] = "CLOSE_WRITE";
static char EVT_IN_CLOSE_NOWRITE[] = "CLOSE_NOWRITE";
static char EVT_IN_ACCESS[] = "ACCESS";
static char EVT_IN_DELETE[] = "DELETE";
static char EVT_IN_MOVED_FROM[] = "MOVED_FROM";
static char EVT_IN_MOVED_TO[] = "MOVED_TO";
static char EVT_IN_MODIFY[] = "MODIFY";
static char EVT_IN_CREATE[] = "CREATE";
static char EVT_IN_UNKNOWN[] = "UNKNOWN";

static char EVT_USER_SWEEP_NEW[] = "USER_SWEEP_NEW";
static char EVT_USER_SWEEP_MOD[] = "USER_SWEEP_MOD";
static char EVT_USER_SWEEP_LOG[] = "USER_SWEEP_LOG";


static const char * inotify_event_name (int event_id)
{
    switch (event_id) {
    case IN_Q_OVERFLOW:
        return EVT_IN_Q_OVERFLOW;
    case IN_DELETE_SELF:
        return EVT_IN_DELETE_SELF;
    case IN_MOVE_SELF:
        return EVT_IN_MOVE_SELF;
    case IN_UNMOUNT:
        return EVT_IN_UNMOUNT;
    case IN_ISDIR:
        return EVT_IN_ISDIR;
    case IN_CLOSE_WRITE:
        return EVT_IN_CLOSE_WRITE;
    case IN_CLOSE_NOWRITE:
        return EVT_IN_CLOSE_NOWRITE;
    case IN_ACCESS:
        return EVT_IN_ACCESS;
    case IN_DELETE:
        return EVT_IN_DELETE;
    case IN_MOVED_FROM:
        return EVT_IN_MOVED_FROM;
    case IN_MOVED_TO:
        return EVT_IN_MOVED_TO;
    case IN_MODIFY:
        return EVT_IN_MODIFY;
    case IN_CREATE:
        return EVT_IN_CREATE;
    case USER_SWEEP_NEW:
        return EVT_USER_SWEEP_NEW;
    case USER_SWEEP_MOD:
        return EVT_USER_SWEEP_MOD;
    case USER_SWEEP_LOG:
        return EVT_USER_SWEEP_LOG;
    default:
        return EVT_IN_UNKNOWN;
    }
}

#endif /* INOTIFYAPI_H_INCLUDED */
