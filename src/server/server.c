/**
* server.c
*
* Init Created: 2016-07-01
* Last Updated: 2016-07-01
*/
#include "server.h"

static char VERSION[] = "0.0.1";


typedef void sigfunc(int);

sigfunc * signal (int signo, sigfunc *func)
{
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;

    if (signo == SIGALRM) {
#ifdef  SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;  /* SunOS 4.x */
#endif
    } else {
#ifdef  SA_RESTART
        act.sa_flags |= SA_RESTART;    /* SVR4, 4.4BSD */
#endif
    }

    if (sigaction (signo, &act, &oact) < 0) {
        return (SIG_ERR);
    } else {
        return (oact.sa_handler);
    }
}


void print_info (const char * cfgfile, const char * log4crc)
{
    printf ("\n***************************************************************");  
    printf ("\n* %s-%s", SERVER_APP_NAME, VERSION);
    printf ("\n* Build: %s %s", __DATE__, __TIME__);
    printf ("\n* Copyright (C) 2016 pepstack.com");
    printf ("\n* Author: master");
    printf ("\n***************************************************************\n");
    printf ("\n* cfg=%s", cfgfile);
    printf ("\n* %s/log4crc\n", log4crc);
}


void print_usage (const char * prog_name)
{
    fprintf (stdout, "\tUsage: %s [options]\n", prog_name);
    fprintf (stdout, "\toptions:\n"
                  "\t\t-d, --daemon              run as daemon process\n"
                  "\t\t-f, --config=<server.xml> set path/to/server.xml'\n"
                  "\t\t-h, --help                display help messages\n"
                  "\t\t-k, --kill                kill all fastsync-server processes\n"
                  "\t\t-l, --list                list of pids for fastsync-server process\n"
                  "\t\t-v, --version             print version\n");
}

/**
* We must catch the SIGCHLD signal when forking child processes.
*
* We must handle interrupted system calls when we catch signals.
*
* A SIGCHLD handler must be coded correctly using waitpid to prevent
*   any zombies from being left around.
*/
int main(int argc, char ** argv)
{
    int ret;

    /* set default config file */
    char cfgxml[MAX_FILENAME_LEN + 1] = {0};
    char log4crc[MAX_FILENAME_LEN + 1] = {0};

    /* parse arguments */
    const struct option lopts[] = {
        {"help", no_argument, 0, 'h'},
        {"daemon", no_argument, 0, 'd'},
        {"config", optional_argument, 0, 'f'},
        {"kill", no_argument, 0, 'k'},
        {"list", no_argument, 0, 'l'},
        {"VERSION", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    time_t start_time = time(0);

    int sessionid = 0;

    while ((ret = getopt_long(argc, argv, "dhklf:v", lopts, 0)) != EOF) {
        switch (ret) {
        case 'd':
            sessionid = 1;
            break;

        case 'h':
            print_usage (argv[0]);
            exit(0);
            break;

        case 'f':
            if (optarg) {
                strncpy(cfgxml, optarg, MAX_FILENAME_LEN);
                cfgxml[MAX_FILENAME_LEN] = 0;
            }
            break;

        case 'k':
            sprintf(cfgxml, "%s-%s", SERVER_APP_NAME, VERSION);
            ret = find_pid_by_name(cfgxml, kill_pid);
            if (ret > 0) {
                fprintf (stdout, "**** total %d processes killed.\n", ret);
            }
            exit(ret);
            break;

        case 'l':
            sprintf(cfgxml, "%s-%s", SERVER_APP_NAME, VERSION);
            ret = find_pid_by_name(cfgxml, list_pid);
            if (ret > 0) {
                fprintf (stdout, "**** total %d processes found.\n", ret);
            }
            exit(ret);
            break;

        case 'v':
            printf("fastsync-server-%s, build:%s %s\n\n", VERSION, __DATE__, __TIME__);
            exit(0);
        }
    }

    do {
        char *p = strrchr(cfgxml, '/');
        *p = 0;

        snprintf(log4crc, MAX_FILENAME_LEN, "LOG4C_RCPATH=%s", cfgxml);
        *p = '/';

        log4crc[MAX_FILENAME_LEN] = 0;
        if (0 != putenv(log4crc)) {
            perror(log4crc);
        }
    } while (0);

    print_info(cfgxml, log4crc);

    LOGGER_INIT();

    LOGGER_INFO("\n\t%s-%s"
        "\n\tBuild: %s %s"
        "\n\tCopyright (C) 2016 pepstack.com"
        "\n\tAuthor: master\n"
        "\n\tconfig: %s"
        "\n\tpid: %d"
        "\n\tstart: %s\n",
        SERVER_APP_NAME, VERSION,
        __DATE__, __TIME__, cfgxml, getpid(), ctime(&start_time));

    LOGGER_FATAL("%s-%s shutdown.\n", SERVER_APP_NAME, VERSION);
    LOGGER_FINI();
    printf ("\n**** %s-%s shutdown.\n\n", SERVER_APP_NAME, VERSION);

    exit(0);
}