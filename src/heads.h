/**
* heads.h
*   common linux heads
*
* Init Created: 2014-08-01
* Last Updated: 2017-01-09
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

#include <unistd.h>          /* close */
#include <fcntl.h>           /* open */
#include <errno.h>           /* errno */
#include <signal.h>

#include <pthread.h>         /* link: -pthread */
#include <sys/types.h>
#include <sys/wait.h>        /* waitpid */
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/sendfile.h>    /* sendfile */
#include <sys/ioctl.h>       /* ioctl, FIONREAD */

#include <dirent.h>          /* open, opendir, closedir, rewinddir, seekdir, telldir, scandir */

#include <time.h>

#include <netinet/in.h>      /* sockaddr_in */
#include <netinet/tcp.h>

#include <arpa/inet.h>       /* inet_addr */
#include <netdb.h>
#include <semaphore.h>
#include <stdarg.h>

#include <getopt.h>          /* getopt_long */
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


#endif /* HEADS_H_INCLUDED */
