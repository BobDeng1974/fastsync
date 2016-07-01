/**
* heads.h
*   common linux heads
*
* Init Created: 2014-08-01
* Last Updated: 2016-07-01
*/
#ifndef HEADS_H_INCLUDED
#define HEADS_H_INCLUDED

#include <string.h>  /* memset */
#include <stdio.h>   /* printf, perror */
#include <limits.h>  /* realpath */

/**
 * MUST include stdlib before jemalloc
 * link: LDFLAGS ?= /usr/local/lib/libjemalloc.a -lpthread
 */
#include <stdlib.h>
#include <jemalloc/jemalloc.h>

#include <unistd.h> /* close */
#include <fcntl.h>  /* open */
#include <errno.h>  /* errno */
#include <signal.h>

#include <pthread.h>  /* link: -pthread */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/sendfile.h> /* sendfile */
#include <sys/ioctl.h>    /* ioctl, FIONREAD */

#include <dirent.h>       /* open, opendir, closedir, rewinddir, seekdir, telldir, scandir */

#include <time.h>

#include <netinet/in.h>  /* sockaddr_in */
#include <netinet/tcp.h>

#include <arpa/inet.h>   /* inet_addr */
#include <netdb.h>
#include <semaphore.h>
#include <stdarg.h>

#include <getopt.h>  /* getopt_long */
#include <regex.h>

/* sqlite3 should be installed first */
#include <sqlite3.h>

/* log4c should be installed first */
#include <log4c.h>

/* common */
#include "./common/byteorder.h"
#include "./common/md5.h"
#include "./common/getmac.h"
#include "./common/hashmap.h"
#include "./common/readconf.h"
#include "./common/threadpool.h"


typedef void (*sighandler_t)(int);

static int pox_system (const char *cmd_line)
{
    int ret = 0;
    sighandler_t old_handler;

    old_handler = signal (SIGCHLD, SIG_DFL);
    ret = system (cmd_line);
    if (ret != 0) {
        perror(cmd_line);
    }
    signal (SIGCHLD, old_handler);

    return ret;
}


typedef int (*find_pid_callback)(int, void*);

struct pid_func_info {
    int counter;
    int thispid;
    const char * path;
};

static int find_pid_by_name (const char* procname, find_pid_callback pid_func)
{
    DIR            *dir;
    struct dirent  *d;
    int            pid, i;
    char           *s;
    int            pnlen;

    struct pid_func_info  pfi = {0, getpid(), 0};

    i = 0;

    pnlen = strlen(procname);

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (! dir) {
        perror("opendir");
        return -1;
    }

    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) {
        char exe [PATH_MAX+1];
        char path[PATH_MAX+1];
        int len;
        int namelen;

        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0) {
            continue;
        }

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if ((len = readlink(exe, path, PATH_MAX)) < 0) {
            continue;
        }

        path[len] = '\0';

        /* find procname */
        s = strrchr(path, '/');
        if(s == NULL) {
            continue;
        }
        s++;

        /* we don't need small name len */
        namelen = strlen(s);
        if (namelen < pnlen) {
            continue;
        }

        if (!strncmp(procname, s, pnlen)) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                pfi.path = path;

                if (! pid_func(pid, (void*) & pfi)) {
                    break;
                }
            }
        }
    }

    closedir(dir);

    return  pfi.counter;
}


static int kill_pid (int pid, void * arg)
{
    struct pid_func_info * pfi = (struct pid_func_info *) arg;

    if (pid != pfi->thispid) {
        int ret = kill (pid, 0);
        if (ret) {
            perror("kill");
        } else {
            ret = kill(pid, SIGTERM);
            if (ret) {
                perror("kill");
            } else {
                fprintf(stdout, "<%d> process(%d) killed: %s\n", ++pfi->counter, pid, pfi->path);
            }
        }
    }

    return 1;
}


static int list_pid (int pid, void * arg)
{
    struct pid_func_info * pfi = (struct pid_func_info *) arg;

    if (pid != pfi->thispid) {
        fprintf(stdout, "<%d> process(%d) found: %s\n", ++pfi->counter, pid, pfi->path);
    }

    return 1;
}

#endif /* HEADS_H_INCLUDED */
