/**
* sockapi.h
*
* Init Created: 2014-08-01
* Last Updated: 2016-07-01
*/
#ifndef SOCKAPI_H_INCLUDED
#define SOCKAPI_H_INCLUDED

#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include <jemalloc/jemalloc.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <pthread.h>  /* link: -pthread */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/sendfile.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <semaphore.h>
#include <stdarg.h>

#include <getopt.h>  /* getopt_long */

/*
 * type definitions
 */
#ifndef SOCKET
    typedef int SOCKET;
#endif

#ifndef ERROR_SOCKET
#   define ERROR_SOCKET (-1)
#endif

#define SUCCESS  0
#define ERROR  (-1)


static int setbuflen (SOCKET s, int rcvlen, int sndlen)
{
    int rcv, snd;
    int rcvl = (int) sizeof(int);
    int sndl = rcvl;

    /* default size is 8192 */
    if ( getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv, &rcvl)==ERROR_SOCKET ||
        getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd, &sndl)==ERROR_SOCKET ) {
        return ERROR_SOCKET;
    }

    if (rcv < rcvlen) {
        rcv = rcvlen;
        rcvl = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcv, rcvl);
    }

    if (snd < sndlen){
        snd = sndlen;
        sndl = setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&snd, sndl);
    }

    return SUCCESS;
}


static int setsocktimeo (SOCKET sockfd, int sendTimeoutSec, int recvTimeoutSec)
{
    if (sendTimeoutSec > 0) {
        struct timeval sndtime = {sendTimeoutSec, 0};
        if (SUCCESS != setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,
                                (char*)&sndtime, sizeof(struct timeval))) {
            return ERROR;
        }
    }

    if (recvTimeoutSec > 0) {
        struct timeval rcvtime = {recvTimeoutSec, 0};
        if (SUCCESS != setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                                (char*)&rcvtime, sizeof(struct timeval))) {
            return ERROR;
        }
    }

    return SUCCESS;
}


static int recvlen (int fd, char * buf, int len)
{
    int    rc;
    char * pb = buf;

    while (len > 0) {
        rc = recv (fd, pb, len, 0);
        if (rc > 0) {
            pb += rc;
            len -= rc;
        } else if (rc < 0) {
            if (errno != EINTR) {
                /* error returned */
                return -1;
            }
        } else {
            /* rc == 0: socket has closed */
            return 0;
        }
    }

    return (pb - buf);
}


static int sendlen (int fd, const char* msg, int len)
{
    int    rc;
    const char * pb = msg;

    while (len > 0) {
        rc = send(fd, pb, len, 0);
        if (rc > 0) {
            pb += rc;
            len -= rc;
        } else if (rc < 0) {
            if (errno != EINTR) {
                /* error returned */
                return -1;
            }
        } else {
            /* socket closed */
            return 0;
        }
    }

    return (pb - msg);
}


static int sendbulky (SOCKET s, const char* bulk, int bulksize,
    int sndl /* sndl: length per msg to send */)
{
    int snd, ret;
    int left = bulksize;
    int at = 0;

    while (left > 0) {
        snd = left > sndl? sndl : left;
        ret = sendlen(s, &bulk[at], snd);
        if (ret != snd) {
            return ret;
        }
        at += ret;
        left -= ret;
    }

    return at;
}


static int recvbulky (SOCKET s, char* bulk, int len,
    int rcvl/* length per msg to recv */)
{
    int  rcv, ret;
    int  left = len;
    int  at = 0;

    while (left > 0) {
        rcv = left > rcvl? rcvl : left;
        ret = recvlen(s, &bulk[at], rcv);
        if (ret != rcv) {
            return ret;
        }
        at += ret;
        left -= ret;
    }

    return at;
}


static SOCKET opensocket (const char *server, int port)
{
    SOCKET sockfd;
    struct sockaddr_in   saddr;
    struct hostent      *hp;
    unsigned int         iaddr;

    hp = 0;

    /* parse saddr host */
    if (inet_addr (server) == INADDR_NONE) {
        hp = gethostbyname (server);
        if(hp == 0) {
            return ERROR_SOCKET;
        }
        saddr.sin_addr.s_addr = *((unsigned long*) hp->h_addr);
    } else {
        iaddr = inet_addr (server);
        saddr.sin_addr.s_addr = iaddr;
    }

    saddr.sin_family = AF_INET;  /* PF_INET */
    saddr.sin_port = htons ((u_short) port);

    /* create a new socket and attempt to connect to saddr */
    sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);  /* PF_INET */
    if (sockfd == ERROR_SOCKET) {
        return ERROR_SOCKET;
    }

    /* make a connection to the saddr, no matter what local port used */
    if ( connect (sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) == ERROR_SOCKET) {
        close ( sockfd );
        return ERROR_SOCKET;
    }

    /* all is ok */
    return sockfd;
}


/**
 * For Linux Client, there are 4 parameters to keep alive for TCP:
 *   tcp_keepalive_probes - detecting times
 *   tcp_keepalive_time - send detecting pkg after last pkg
 *   tcp_keepalive_intvl - interval between detecting pkgs
 *   tcp_retries2 -
 * On Linux OS, use "echo" to update these params:
 *   echo "30" > /proc/sys/net/ipv4/tcp_keepalive_time
 *   echo "6" > /proc/sys/net/ipv4/tcp_keepalive_intvl
 *   echo "3" > /proc/sys/net/ipv4/tcp_keepalive_probes
 *   echo "3" > /proc/sys/net/ipv4/tcp_retries2
 * tcp_keepalive_time and tcp_keepalive_intvl is by seconds
 *   if keep them after system reboot, must add to: /etc/sysctl.conf file
 */
static int setsockalive (SOCKET s, int keepIdle, int keepInterval, int keepCount)
{
    int keepAlive = 1;
    socklen_t len = sizeof(keepAlive);

    /* enable keep alive */
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));

    // tcp idle time, test time internal, test times
    if (keepIdle > 0) {
        setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
    }
    if (keepInterval > 0) {
        setsockopt(s, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
    }
    if (keepCount > 0) {
        setsockopt(s, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
    }

    /* Check the status again */
    if(getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, &len) < 0) {
        return ERROR_SOCKET;
    }

    return keepAlive;
}


static ssize_t pread_len (int fd, char * buf, size_t len, off_t pos)
{
    ssize_t rc;
    char * pb = buf;

    while (len != 0 && (rc = pread (fd, (void*) pb, len, pos)) != 0) {
        if (rc == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            } else {
                /* pread error */
                return ERROR;
            }
        }

        len -= rc;
        pos += rc;
        pb += rc;
    }

    return (ssize_t) (pb - buf);
}


static ssize_t sendfile_len (int sockfd, int filefd, size_t len, off_t pos)
{
    ssize_t rc;

    off_t  off = pos;

    while (len != 0 && (rc = sendfile (sockfd, filefd, &off, len)) != 0) {
        if (rc == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            } else {
                return -1;
            }
        }

        /* rc > 0 */
        len -= rc;
    }

    return (ssize_t) (off - pos);
}

#endif /* SOCKAPI_H_INCLUDED */
