/**
* msgs.h
*
* Init Created: 2016-07-06
* Last Updated: 2016-07-06
*/
#ifndef MSGS_H_INCLUDED
#define MSGS_H_INCLUDED

#include <unistd.h>  /* readlink */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "./common/byteorder.h"

#define FSYNC_MAGIC_LEN               4
#define FSYNC_PATHPREFIX_LEN        255
#define FSYNC_DEFAULT_BACKLOG        16
#define FSYNC_MD5SUM_LEN             32
#define FSYNC_PKGNAME_LEN            60

/**
* trim specified character in given string
*/
static char* trim (char *s, char c)
{
    return (*s==0)?s:(((*s!=c)?(((trim(s+1,c)-1)==s)?s:(*(trim(s+1,c)-1)=*s,*s=c,trim(s+1,c))):trim(s+1,c)));
}


static char * strrplchr (char *s, char c, char d)
{
    char * p = s;
    while (*p) {
        if (*p == c) {
            *p = d;
        }
        ++p;
    }
    return s;
}


#endif /* MSGS_H_INCLUDED */
