/**
 * client.h
 *
 * 2016-07-20: init created
 * 2017-01-14: last updated
 */
#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#define ARGS_COUNT_MAX  20
#include "../args.h"

#include "client_conf.h"


static char VERSION[] = "0.0.1";

static char semaphore[] = FSYNC_CLIENT_APP;

static char cfgfile[MAX_FILENAME_LEN + 1];

static char log4crc[MAX_FILENAME_LEN + 1];
static char priority[10];
static char appender[10];


/**
 * print usage for program
 *
 * ------ color output ------
 * 30: black
 * 31: red
 * 32: green
 * 33: yellow
 * 34: blue
 * 35: purple
 * 36: dark green
 * 37: white
 * --------------------------
 * "\033[31m RED   \033[0m"
 * "\033[32m GREEN \033[0m"
 * "\033[33m YELLOW \033[0m"
 */
void print_usage(void)
{
    printf("\033[47;35mUsage:\033[0m %s <express> [Options]\n", FSYNC_CLIENT_APP);
    printf("\t\033[35mFastly synchronize files from client to server(s).\033[0m\n");
    printf("\033[47;35mOptions:\033[0m\n"
        "\t-h, --help                  \033[35mdisplay help messages\033[0m\n"
        "\t-V, --version               \033[35mprint version information\033[0m\n"
        "\t-v, --verbose               \033[35moutput verbose messages\033[0m\n"
        "\n"
        "\t-C, --config=XMLFILE        \033[35mspecify config xml path file. './conf/client-cfg.xml' (default)\033[0m\n"
        "\t-O, --log4crc=PATH          \033[35mspecify dir of log4crc file. './conf' (default)\033[0m\n"
        "\t-P, --priority=<PRIORITY>   \033[35moverwrite priority in log4crc, available PRIORITY:\033[0m\n"
        "\t                                      \033[35m'fatal'\033[0m\n"
        "\t                                      \033[35m'error' - used in stable release stage\033[0m\n"
        "\t                                      \033[35m'warn'\033[0m\n"
        "\t                                      \033[35m'info'  - used in release stage\033[0m\n"
        "\t                                      \033[35m'debug' - used only in devel\033[0m\n"
        "\t                                      \033[35m'trace' - show all details\033[0m\n"
        "\t-A, --appender=<APPENDER>   \033[35moverwrite appender in log4crc, available APPENDER:\033[0m\n"
        "\t                                      \033[35m'default' - using appender specified in log4crc\033[0m\n"
        "\t                                      \033[35m'stdout' - using appender stdout\033[0m\n"
        "\t                                      \033[35m'stderr' - using appender stderr\033[0m\n"
        "\t                                      \033[35m'syslog' - using appender syslog\033[0m\n"
        "\n"
        "\t-D, --daemon                \033[35mrun as daemon process\033[0m\n"
        "\t-K, --kill                  \033[35mkill all processes for this program\033[0m\n"
        "\t-L, --list                  \033[35mlist of pids for this program\033[0m\n"
        "\n"
        "\t-m, --md5=FILE              \033[35mmd5sum on given FILE\033[0m\n"
        "\t-r, --regexp=PATTERN        \033[35muse pattern for matching on <express>\033[0m\n"
        "\n"
        "\033[47;35m%s, Version: %s, Build: %s %s. Copyright (C) 2017 pepstack.com, ALL RIGHTS RESERVED.\033[0m\n",
        FSYNC_CLIENT_APP, VERSION, __DATE__, __TIME__);
}


void print_info (const char * cfgfile, const char * log4crc, time_t start_time, int isdaemon)
{
    printf("\n\e[1;34m***************************************************************\e[0m");
    printf("\n\e[1;34m* %s-%s\e[0m", FSYNC_CLIENT_APP, VERSION);
    printf("\n\e[1;34m* Build: %s %s\e[0m", __DATE__, __TIME__);
    printf("\n\e[1;34m* Copyright (C) 2017 pepstack.com\e[0m");
    printf("\n\e[1;34m* Author: master@pepstack.com\e[0m");
    printf("\n\e[1;34m***************************************************************\e[0m\n");
    printf("\n* Main process: %d", getpid());
    printf("\n* %s", log4crc);
    printf("\n* Config File: %s", cfgfile);
    printf("\n* Run as daemon: %s", isdaemon ? "Yes" : "No");
    printf("\n* Start time: %s\n", ctime(&start_time));
}


void set_default_config(char *cfgfile, int sizecfgfile, char *log4crc, int sizelog4crc)
{
    /* get default real client-cfg.xml path */
    int len = getpwd(cfgfile, sizecfgfile);
    if (! len) {
        fprintf(stderr, "error: %s\n\n", cfgfile);
        exit(-1);
    }
    if (len + strlen("conf/client-cfg.xml") >= sizecfgfile ||
        len + strlen("LOG4C_RCPATH=") >= sizelog4crc) {
        fprintf(stderr, "error: application path is too long: %s\n\n", cfgfile);
        exit(-1);
    }

    /* set env for LOG4C */
    snprintf(log4crc, sizelog4crc, "LOG4C_RCPATH=%sconf", cfgfile);

    snprintf(cfgfile + len, sizecfgfile - len, "conf/client-cfg.xml");
}


static int get_startcmd (int argc, char ** argv, char * cmd, int size)
{
    int i, ret = 0;

    char binpath[MAX_FILENAME_LEN + 1];

    char * p = strrchr(argv[0], '/');

    /* get abs dir to bin */
    if (! get_bindir(argv[0], cmd, size)) {
        LOGGER_ERROR("error(%d): %s", errno, strerror(errno));
    }

    if (p) {
        if (strcmp(p+1, FSYNC_CLIENT_APP)) {
            /* NOT a slink, must create a slink to real bin file */
            strcpy(binpath, cmd);
            strcat(binpath, p);
            binpath[MAX_FILENAME_LEN] = 0;

            /* slink path */
            strcat(cmd, "/");
            strcat(cmd, FSYNC_CLIENT_APP);

            if (symlink(binpath, cmd) == 0 || errno == EEXIST) {
                LOGGER_INFO("symlink created for: %s->%s", cmd, binpath);
            } else {
                LOGGER_FATAL("symlink error(%d): %s", errno, strerror(errno));
                return ERROR;
            }
        } else {
            /* IS a slink */
            strcat(cmd, p);
        }
    } else {
        if (strcmp(argv[0], FSYNC_CLIENT_APP)) {
            /* NOT a slink, must create a slink to real bin file */
            strcpy(binpath, cmd);
            strcat(binpath, p);
            binpath[MAX_FILENAME_LEN] = 0;

            /* slink path */
            strcat(cmd, "/");
            strcat(cmd, FSYNC_CLIENT_APP);

            if (symlink(binpath, cmd) == 0 || errno == EEXIST) {
                LOGGER_INFO("symlink created for: %s=>%s", cmd, binpath);
            } else {
                LOGGER_ERROR("symlink error(%d): %s", errno, strerror(errno));
                return ERROR;
            }
        } else {
            /* IS a slink */
            strcat(cmd, "/");
            strcat(cmd, argv[0]);
        }
    }

    ret = strlen(cmd);

    for (i = 1; i < argc; ++i) {
        strcat(cmd, " ");
        strcat(cmd, argv[i]);
    }

    return ret;
}


static void md5file (const char * filename)
{
    char md5str[MD5_HASH_SIZE];
    MD5HashFile(filename, 0, md5str);
    printf ("md5='%s', file='%s'\n", md5str, filename);
}


static char * substr(const char * str, unsigned start, unsigned end, char * buf, int buflen)
{
    unsigned len = end - start;
    if (len >= buflen) {
        len = buflen - 1;
    }
    strncpy (buf, str + start, len);
    buf[len] = 0;
    return buf;
}


static void regexp (const char * pattern, const char * express)
{
    regex_t reg;
    char out[256];
    int err;

    printf ("regexp: pattern='%s', express='%s'\n", pattern, express);

    err = regcomp (& reg, pattern, REG_EXTENDED);
    if (err != 0) {
        regerror(err, & reg, out, sizeof(out));
        printf ("regcomp failed: %s\n", out);
    } else {
        regmatch_t pm[20];
        const size_t nmatch = 20;

        err = regexec (& reg, express, nmatch, pm, REG_EXTENDED);
        if (err == REG_NOMATCH) {
            printf ("no matched\n");
        } else if (err == 0) {
            int x;
            for (x = 0; x < nmatch && pm[x].rm_so != -1; ++x) {
                printf("matched: '%s'\n", substr(express, pm[x].rm_so, pm[x].rm_eo, out, sizeof(out)));
            }
        } else {
            regerror (err, & reg, out, sizeof(out));
            printf ("regexec failed: %s\n", out);
        }

        regfree (& reg);
    }
}



static void sweep_thread_func (void *arg)
{

}


/**
 * Create a sweep thread for readdir
 */
static int start_sweep_thread (pthread_t * ptid, client_conf_t * client)
{
    int err = ERROR;

    pthread_attr_t pattr;

    pthread_attr_init(&pattr);
    pthread_attr_setscope(&pattr, PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);

    err = pthread_create(ptid, &pattr, (void *) sweep_thread_func, (void*) client);
    if (err != 0) {
        LOGGER_FATAL("pthread_create() error: %s", strerror(errno));
    }
    pthread_attr_destroy(& pattr);

    return err;
}


static int handle_event (struct inotify_event *event, client_conf_t *client)
{
    int handled = 0;
    int eventid = -1;

    if (event->mask == IN_Q_OVERFLOW) {
        eventid = IN_Q_OVERFLOW;
        return (-6);
    }

    if (event->mask & IN_DELETE_SELF) {
        eventid = IN_DELETE_SELF;
    }

    if (event->mask & IN_MOVE_SELF) {
        eventid = IN_MOVE_SELF;
    }

    if (event->mask & IN_UNMOUNT) {
        eventid = IN_UNMOUNT;
    }

    if (event->mask & IN_ISDIR) {
        eventid = IN_ISDIR;
    }

    if (event->mask & IN_MODIFY) {
        eventid = IN_MODIFY;

        if (event->len > 0) {
            ///handled = threadpool_add_event_task(eventid, client, event);
            return handled;
        }
    } else if (event->mask & IN_CLOSE_WRITE) {
        eventid = IN_CLOSE_WRITE;
        if (event->len > 0) {
            ///handled = threadpool_add_event_task(eventid, client, event);
            return handled;
        }
    }

    if (event->mask & IN_DELETE) {
        eventid = IN_DELETE;
        if (event->len > 0) {
            ///handled = threadpool_add_event_task(eventid, client, event);
        }
    }

    if (event->mask & IN_CREATE) {
        eventid = IN_CREATE;
    }

    if (event->mask & IN_MOVED_FROM) {
        eventid = IN_MOVED_FROM;
    }

    if (event->mask & IN_MOVED_TO) {
        eventid = IN_MOVED_TO;
    }

    if (event->mask & IN_ACCESS) {
        /* md5sum will cause this event, so just ignore it */
        eventid = IN_ACCESS;
    }

    return handled;
}


#if defined(__cplusplus)
}
#endif

#endif /* CLIENT_H_INCLUDED */
