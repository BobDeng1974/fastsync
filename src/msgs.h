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

/**
* ipv4: xxx.xxx.xxx.xxx (maxlen=15)
* ipv6: xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx (maxlen=39)
* mac addr: xx-xx-xx-xx-xx-xx (fixlen=17)
* datetime: YYYY-mm-dd HH:MM:SS (fixlen=19)
* md5: a967bcb89d5b3b9f959e0a7c39a03d81 (fixlen=32)
*/
#define IPV4_MAXLEN     15
#define IPV6_MAXLEN     39
#define MACADDR_LEN     17
#define DATETIME_LEN    19
#define MD5SUM_LEN      32


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
