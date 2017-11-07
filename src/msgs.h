/**
* msgs.h
*
* Init Created: 2016-07-06
* Last Updated: 2017-01-09
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


#ifndef MAX_FILENAME_LEN
#define MAX_FILENAME_LEN            255
#endif

#define TRUE     1
#define FALSE    0

#define SUCCESS     0
#define ERROR     (-1)
#define EMEMORY   (-10)


/**
 * trim specified character in given string
 */
static char* trim (char *s, char c)
{
    return (*s==0)?s:(((*s!=c)?(((trim(s+1,c)-1)==s)?s:(*(trim(s+1,c)-1)=*s,*s=c,trim(s+1,c))):trim(s+1,c)));
}


/**
 * trim specified characters in given string
 */
static char* trims (char *s, char *chrs)
{
    char *p = chrs;
    while (*p != 0) {
        s = trim(s, *p++);
    }
    return s;
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


static char *strlwr(char *s)
{
    char *p;
    p = s;

    while(*p != '\0') {
        if (*p >= 'A' && *p <= 'Z') {
            *p = (*p) + 0x20;
        }
        p++;
    }
    return s;
}


static int str_in_array(const char * str, const char ** argv)
{
    int i = 0;

    while (argv[i]) {
        if (! strcmp(str, argv[i++])) {
            return i;
        }
    }

    return 0;
}


#endif /* MSGS_H_INCLUDED */
