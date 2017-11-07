/**
* server_conf.c
*
* Init Created: 2016-07-06
* Last Updated: 2017-01-06
*/
#include "server_conf.h"
#include "regdb.h"


static const char * forbiden_prefixs[] = {
    "/bin/",
    "/cgroup/",
    "/etc/",
    "/lib/",
    "/lost/",
    "/lost+found/",
    "/media/"
    "/misc/",
    "/net/",
    "/proc/",
    "/sbin/",
    "/srv/",
    "/boot/",
    "/dev/",
    "/lib64/",
    "/initrd/",
    "/root/",
    "/run/",
    "/selinux/",
    "/sys/",
    "/usr/",
    "/tmpfs/",
    "/service/",
    "/command/",
    0
};


int server_conf_create(const char * cfgfile, server_conf_t **ppOut)
{
    int err = 0;

    server_conf_t * p = 0;
    mxml_node_t *tree = 0;
    FILE *fp = 0;

    *ppOut = 0;

    do {
        mxml_node_t *root;
        mxml_node_t *node;

        fp = fopen(cfgfile, "r");
        if (! fp) {
            return -1;
        }

        tree = mxmlLoadFile(0, fp, MXML_TEXT_CALLBACK);
        if (! tree) {
            fclose(fp);
            return -2;
        }

        root = mxmlFindElement(tree, tree, "fastsync-server", 0, 0, MXML_DESCEND);
        CHECK_NODE_ERR(root, -3)

        if (safe_stricmp(mxmlElementGetAttr(root, "xmlns"), "http://pepstack.com")) {
            err = -4;
            goto ERROR_RET;
        }

        p = mem_alloc(1, sizeof(server_conf_t));
        MxmlNodeGetStringAttr(root, "ver", p->ver, sizeof(p->ver));

        /* authentication */
        do {
            mxml_node_t *subnode;

            node = mxmlFindElement(root, root, "authentication", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -5)

            subnode = mxmlFindElement(node, node, "magic", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(subnode, -6)

            if (mxmlGetText(subnode, 0)) {
                strncpy(p->magic, mxmlGetText(subnode, 0), sizeof(p->magic));
                p->magic[FSYNC_MAGIC_LEN] = 0;
            }

            node = mxmlFindElement(node, node, "ssl", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -6)

            MxmlNodeGetIntegerAttr(node, "enabled", &p->ssl_enabled);

            if (p->ssl_enabled) {
                subnode = mxmlFindElement(node, node, "pubkey", 0, 0, MXML_DESCEND);
                CHECK_NODE_ERR(subnode, -7)

                subnode = mxmlFindElement(node, node, "privkey", 0, 0, MXML_DESCEND);
                CHECK_NODE_ERR(subnode, -8)
            }
        } while(0);

        /* stash */
        do {
            mxml_node_t *subnode;

            node = mxmlFindElement(root, root, "stash", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -10)

            subnode = mxmlFindElement(node, node, "pathprefix", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(subnode, -11)

            if (mxmlGetText(subnode, 0)) {
                char *psz;
                const char ** ppre;

                strncpy(p->pathprefix, mxmlGetText(subnode, 0), sizeof(p->pathprefix));
                p->pathprefix[FSYNC_PATHPREFIX_LEN] = 0;

                /* check pathprefix */
                if (p->pathprefix[0] != '/' || p->pathprefix[1] == '/' || p->pathprefix[1] == 32) {
                    /* illegal path */
                    err = -12;
                    goto ERROR_RET;
                }

                /* truncate end path separator char */
                do {
                    psz = strrchr(p->pathprefix, '/');
                    if (*++psz == 0) {
                        *psz = 0;
                    }
                } while(*psz == 0);

                if (strlen(p->pathprefix) < 8) {
                    /* pathprefix is too short */
                    err = -13;
                    goto ERROR_RET;
                }

                ppre = forbiden_prefixs;
                while (*ppre) {
                    if (strstr(p->pathprefix, *ppre)) {
                        /* forbiden prefix */
                        err = -14;
                        goto ERROR_RET;
                    }
                    ppre++;
                }

                if (0 != access(p->pathprefix, R_OK|W_OK)) {
                    err = -15;
                    LOGGER_ERROR("access() failed: [errno:%d] %s - '%s'", errno, strerror(errno), p->pathprefix);
                    goto ERROR_RET;
                }
            }

            /* TODO: */
            node = mxmlFindElement(node, node, "backend", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -16)

            subnode = mxmlFindElement(node, node, "maxretry", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(subnode, -17)

            subnode = mxmlFindElement(node, node, "interval", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(subnode, -18)
        } while(0);

        /* connection */
        do {
            mxml_node_t *subnode;

            int second_factor = 1;
            int byte_factor = 1;

            char tmpbuf[20];

            node = mxmlFindElement(root, root, "connection", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -20)

            if (MxmlNodeGetStringAttr(node, "timeunit", tmpbuf, sizeof(tmpbuf))) {
                if (
                    ! safe_stricmp(tmpbuf, "millisecond") ||
                    ! safe_stricmp(tmpbuf, "msec") ||
                    ! safe_stricmp(tmpbuf, "ms")) {
                    second_factor = 1000;
                } else if (
                    ! safe_stricmp(tmpbuf, "second") ||
                    ! safe_stricmp(tmpbuf, "sec") ||
                    ! safe_stricmp(tmpbuf, "s")) {
                    second_factor = 1;
                } else {
                    err = -21;
                    goto ERROR_RET;
                }
            }

            if (MxmlNodeGetStringAttr(node, "sizeunit", tmpbuf, sizeof(tmpbuf))) {
                if (
                    ! safe_stricmp(tmpbuf, "megabyte") ||
                    ! safe_stricmp(tmpbuf, "mbyte") ||
                    ! safe_stricmp(tmpbuf, "mb")) {
                    byte_factor = 1024*1024;
                } else if (
                    ! safe_stricmp(tmpbuf, "kilobyte") ||
                    ! safe_stricmp(tmpbuf, "kbyte") ||
                    ! safe_stricmp(tmpbuf, "kb")) {
                    byte_factor = 1024;
                } else if (
                    ! safe_stricmp(tmpbuf, "byte") ||
                    ! safe_stricmp(tmpbuf, "b")) {
                    byte_factor = 1;
                } else {
                    err = -22;
                    goto ERROR_RET;
                }
            }

            if (! MxmlNodeGetIntegerAttr(node, "backlog", &p->backlog)) {
                p->backlog = FSYNC_DEFAULT_BACKLOG;
            }

            node = mxmlFindElement(root, root, "iptable", 0, 0, MXML_DESCEND);
            if (node) {
                subnode = mxmlFindElement(node, node, "whitelist", 0, 0, MXML_DESCEND);
                if (subnode) {
                    int tmpval = 0;
                    MxmlNodeGetIntegerAttr(subnode, "enabled", &tmpval);
                    if (tmpval && mxmlGetText(subnode, 0)) {
                        tmpval = (int) strlen(mxmlGetText(subnode, 0));
                        p->iptable_whitelist = (char*) mem_alloc(1, tmpval + 2);
                        strncpy(p->iptable_whitelist, mxmlGetText(subnode, 0), tmpval);
                    }
                }

                subnode = mxmlFindElement(node, node, "blacklist", 0, 0, MXML_DESCEND);
                if (subnode) {
                    int tmpval = 0;
                    MxmlNodeGetIntegerAttr(subnode, "enabled", &tmpval);
                    if (tmpval && mxmlGetText(subnode, 0)) {
                        tmpval = (int) strlen(mxmlGetText(subnode, 0));
                        p->iptable_blacklist = (char*) mem_alloc(1, tmpval + 2);
                        strncpy(p->iptable_blacklist, mxmlGetText(subnode, 0), tmpval);
                    }
                }
            }

            node = mxmlFindElement(root, root, "port", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -30)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -31)
            p->port = (unsigned short) atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "maxclients", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -32)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -33)
            p->maxclients = atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "bufsize", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -34)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -35)
            p->bufsize = byte_factor * atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "timeout", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -36)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -37)
            p->timeout = second_factor * atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "keepinterval", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -38)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -39)
            p->keepinterval = second_factor * atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "keepidle", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -40)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -41)
            p->keepidle = second_factor * atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "keepcount", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -42)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -43)
            p->keepcount = atoi(mxmlGetText(node, 0));

            node = mxmlFindElement(root, root, "nodelay", 0, 0, MXML_DESCEND);
            CHECK_NODE_ERR(node, -44)
            CHECK_NODE_ERR(mxmlGetText(node, 0), -45)

            parse_bool_asint(mxmlGetText(node, 0), &p->nodelay);
        } while(0);

        /* autoupdate */
        do {
            node = mxmlFindElement(root, root, "autoupdate", 0, 0, MXML_DESCEND);
            if (node) {
                MxmlNodeGetIntegerAttr(node, "enabled", &p->autoupdate);
                if (p->autoupdate) {
                    mxml_node_t *subnode;

                    subnode = mxmlFindElement(node, node, "clientpkg", 0, 0, MXML_DESCEND);
                    CHECK_NODE_ERR(subnode, -50)

                    MxmlNodeGetStringAttr(subnode, "md5sum",
                        p->clientpkg_md5sum, sizeof(p->clientpkg_md5sum));
                    p->clientpkg_md5sum[MD5SUM_LEN] = 0;

                    strncpy(p->clientpkg, mxmlGetText(subnode, 0), FSYNC_PKGNAME_LEN);
                    p->clientpkg[FSYNC_PKGNAME_LEN] = 0;
                }
            }
        } while(0);

        /**
         * open or create regdb
         */
        if (SUCCESS != regdb_open(p->pathprefix, &p->regdb)) {
            err = -60;
            goto ERROR_RET;
        }

        /* load config xml completed */
        mxmlDelete(tree);
        fclose(fp);
    } while(0);

    *ppOut = p;
    return 0;

ERROR_RET:
    mem_free((void**) &p);
    mxmlDelete(tree);
    fclose(fp);

    return err;
}


void server_conf_free (server_conf_t **server)
{
    server_conf_t *p = *server;
    if (p) {
        regdb_close(&(p->regdb));
        mem_free((void**) server);
    }
}


int server_conf_setsocketopt (int sockfd, const server_conf_t * srvconf)
{
    int nodelay = srvconf->nodelay;

    /* set timeouts on current socket connection */
    if (SUCCESS != setsocktimeo(sockfd, srvconf->timeout, srvconf->timeout)) {
        LOGGER_ERROR("setsocktimeo() failed: [errno:%d] %s", errno, strerror(errno));
        return ERROR;
    }

    /* set keepalive for current socket connection */
    if (1 != setsockalive(sockfd, srvconf->keepidle, srvconf->keepinterval, srvconf->keepcount)) {
        LOGGER_ERROR("setsockalive() failed: [errno:%d] %s", errno, strerror(errno));
        return ERROR;
    }

    /* nodelay=1 disable Nagle, use TCP_CORK to enable Nagle */
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay))) {
        LOGGER_ERROR("setsockopt() failed: [errno:%d] %s", errno, strerror(errno));
        return ERROR;
    }

    return SUCCESS;
}


void server_conf_print (server_conf_t * srvconf)
{
    printf("\tver='%s'\n", srvconf->ver);
    printf("\tmagic='%s'\n", srvconf->magic);
    printf("\tpathprefix='%s'\n", srvconf->pathprefix);
    printf("\tregdb='%s/.regdb'\n", srvconf->pathprefix);

    if (srvconf->iptable_whitelist) {
        printf("\twhitelist='%s'\n", srvconf->iptable_whitelist);
    }
    if (srvconf->iptable_blacklist) {
        printf("\tblacklist='%s'\n", srvconf->iptable_blacklist);
    }

    printf("\tbacklog=%d\n", srvconf->backlog);
    printf("\tport=%d\n", srvconf->port);
    printf("\tmaxclients=%d\n", srvconf->maxclients);
    printf("\tbufsize=%d bytes\n", srvconf->bufsize);
    printf("\ttimeout=%d seconds\n", srvconf->timeout);
    printf("\tkeepinterval=%d seconds\n", srvconf->keepinterval);
    printf("\tkeepidle=%d seconds\n", srvconf->keepidle);
    printf("\tkeepcount=%d\n", srvconf->keepcount);
    printf("\tnodelay=%d\n", srvconf->nodelay);

    printf("\tautoupdate=%d\n", srvconf->autoupdate);
    if (srvconf->autoupdate) {
        printf("\tclientpkg='%s'\n", srvconf->clientpkg);
        printf("\tmd5sum='%s'\n", srvconf->clientpkg_md5sum);
    }
}
