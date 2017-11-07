/**
* server.c
*
* Init Created: 2016-07-01
* Last Updated: 2017-01-06
*/
#include "server.h"


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


void sig_chld (int signo)
{
    pid_t    pid;
    int      stat;

    /* Must call waitpid():
     * We must specify the WNOHANG option: This tells waitpid
     *   not to block if there are running children that have
     *   not yet terminated.
     */
    while ( (pid = waitpid(-1, & stat, WNOHANG)) > 0) {
        printf("\n<SIGCHLD> [%d] child process terminated.\n", pid);
    }

    return;
}


/**
* kill -15 pid
*/
void sig_term (int signo)
{
    void pr_cpu_time (void);

    /* print cpu time */
    pr_cpu_time ();

    exit (signo);
}


/**
* kill -2 pid
*/
void sig_int (int signo)
{
    void pr_cpu_time (void);

    /* print cpu time */
    pr_cpu_time ();

    exit (signo);
}


int start_timer (timer_t *timerid, int start_secs, int interval_secs)
{
    int ret;
    timer_t timer;
    struct sigevent evp;
    struct itimerspec ts;

    evp.sigev_value.sival_ptr = & timer;
    evp.sigev_notify = SIGEV_SIGNAL;
    evp.sigev_signo = SIGUSR1;

    *timerid = 0;

    ret = timer_create (CLOCK_REALTIME, & evp, & timer);
    if (ret) {
        LOGGER_FATAL("timer_create error(%d): %s", errno, strerror(errno));
        return ERROR;
    }

    ts.it_interval.tv_sec = interval_secs;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = start_secs;
    ts.it_value.tv_nsec = 0;

    ret = timer_settime (timer, 0, &ts, NULL);
    if (ret) {
        LOGGER_FATAL("timer_settime error(%d): %s", errno, strerror(errno));
        return -2;
    }

    *timerid = timer;

    return SUCCESS;
}


#ifndef HAVE_GETRUSAGE_PROTO
int getrusage(int, struct rusage *);
#endif


void pr_cpu_time(void)
{
    double user, sys;
    struct rusage myusage, childusage;

    if (getrusage(RUSAGE_SELF, &myusage) < 0) {
        printf ("getrusage error\n");
    }

    if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
        printf ("getrusage error\n");
    }

    user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec / 1000000.0;
    user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / 1000000.0;
    sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec / 1000000.0;
    sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec / 1000000.0;

    printf ("\n* user-time = %g, sys-time = %g\n", user, sys);
}


void print_usage (const char * prog_name)
{
    fprintf (stdout, "\tUsage: %s [options]\n", prog_name);
    fprintf (stdout, "\toptions:\n"
                  "\t\t-d, --daemon              run as daemon process\n"
                  "\t\t-f, --config=<server-cfg.xml>  set path/to/server-cfg.xml'\n"
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
    int len, ret, connfd;

    pid_t   childpid;
    socklen_t clilen;

    struct sockaddr_in cliaddr, srvaddr;

    void sig_chld(int);
    void sig_int(int);
    void sig_term(int);

    /* child process counter */
    int num_childs = 0;

    server_conf_t * server = 0;

    /* set default config file */
    char cfgfile[MAX_FILENAME_LEN + 1] = {0};
    char log4crc[MAX_FILENAME_LEN + 1] = {0};

    int listenfd = ERROR_SOCKET;

    int sessionid = 0;

    time_t start_time = time(0);

    /* command arguments */
    const struct option lopts[] = {
        {"help", no_argument, 0, 'h'},
        {"daemon", no_argument, 0, 'd'},
        {"config", optional_argument, 0, 'f'},
        {"kill", no_argument, 0, 'k'},
        {"list", no_argument, 0, 'l'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    set_default_config(cfgfile, sizeof(cfgfile), log4crc, sizeof(log4crc));

    /* parse command arguments */
    while ((ret = getopt_long(argc, argv, "dhklf:v", lopts, 0)) != EOF) {
        switch (ret) {
        case 'd':
            /* if run as daemon process */
            sessionid = 1;
            break;

        case 'h':
            print_usage (argv[0]);
            exit(0);
            break;

        case 'f':
            if (optarg) {
                strncpy(cfgfile, optarg, MAX_FILENAME_LEN);
                cfgfile[MAX_FILENAME_LEN] = 0;
            }
            break;

        case 'k':
            sprintf(cfgfile, "%s-%s", FSYNC_SERVER_APP, VERSION);
            ret = find_pid_by_name(cfgfile, kill_pid);
            if (ret > 0) {
                fprintf(stdout, "**** total %d processes killed.\n", ret);
            }
            exit(ret);
            break;

        case 'l':
            sprintf(cfgfile, "%s-%s", FSYNC_SERVER_APP, VERSION);
            ret = find_pid_by_name(cfgfile, list_pid);
            if (ret > 0) {
                fprintf(stdout, "**** total %d processes found.\n", ret);
            }
            exit(ret);
            break;

        case 'v':
            printf("%s-%s, build:%s %s\n\n", FSYNC_SERVER_APP, VERSION, __DATE__, __TIME__);
            exit(0);
        }
    }

    print_info(cfgfile, log4crc, start_time);

    LOGGER_INIT();

    LOGGER_INFO("\n\t%s-%s"
        "\n\tBuild: %s %s"
        "\n\tCopyright (C) 2016 pepstack.com"
        "\n\tAuthor: master\n"
        "\n\tconfig: %s"
        "\n\tpid: %d"
        "\n\tstart: %s\n",
        FSYNC_SERVER_APP, VERSION,
        __DATE__, __TIME__, cfgfile, getpid(), ctime(&start_time));

    if (check_file_error(cfgfile, R_OK) != 0) {
        printf("invalid config file: %s\n", cfgfile);
        exit(-1);
    }

    if (sessionid) {
        /* runs in background */
        ret = daemon (0, 0);
        if (ret != 0) {
            perror("daemon error");
            LOGGER_FATAL("daemon error(%d): %s", errno, strerror(errno));
            exit(errno);
        } else {
            LOGGER_INFO("%s-%s is running as daemon process(%d)...",
                FSYNC_SERVER_APP, VERSION, getpid());
        }
    }

    if (signal(SIGCHLD, sig_chld) == SIG_ERR) {
        LOGGER_FATAL("signal(SIGCHLD) failed: %s", strerror(errno));
        exit(-1);
    }

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        LOGGER_FATAL("signal(SIGINT) failed: %s", strerror(errno));
        exit(-1);
    }

    if (signal(SIGTERM, sig_term) == SIG_ERR) {
        LOGGER_FATAL("signal(SIGTERM) failed: %s", strerror(errno));
        exit(-1);
    }

    /* Unix supports the principle of piping, which allows processes to send data
     * to other processes without the need for creating temporary files. When a
     * pipe is broken, the process writing to it is sent the SIGPIPE signal.
     * The default reaction to this signal for a process is to terminate.
     */
    signal(SIGPIPE, SIG_IGN);

    ret = server_conf_create(cfgfile, &server);
    if (ret != 0) {
        LOGGER_FATAL("server_conf_create() failed: %d", ret);
        goto APP_EXIT_ERROR;
    }

    server_conf_print(server);

    /* create socket for incoming connections */
    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd == ERROR_SOCKET) {
        LOGGER_FATAL("socket error(%d): %s", errno, strerror(errno));
        goto APP_EXIT_ERROR;
    }

    do {
        int opt = 1;
        ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (ret) {
            LOGGER_FATAL("setsockopt error(%d): %s", errno, strerror(errno));
            goto APP_EXIT_ERROR;
        }
    } while (0);

    /* construct srvaddr address structure */
    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvaddr.sin_port = htons(DEFAULT_PORT);

    /* bind to the srvaddr address */
    ret = bind(listenfd, (struct sockaddr *) &srvaddr, sizeof(srvaddr));
    if (ret == ERROR_SOCKET) {
        LOGGER_FATAL("bind() failed: %s", strerror(errno));
        goto APP_EXIT_ERROR;
    }

    /* MUST set recv and send timeout before listen */
    ret = setsocktimeo(listenfd, 6, 6);
    if (ret != SUCCESS) {
        LOGGER_FATAL("setsocktimeo() failed: %s", strerror(errno));
        goto APP_EXIT_ERROR;
    }

    /* nodelay=1 disable Nagle, use TCP_CORK to enable Nagle */
    if (setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &server->nodelay, sizeof(server->nodelay))) {
        LOGGER_FATAL("setsockopt TCP_NODELAY error(%d): %s", errno, strerror(errno));
        goto APP_EXIT_ERROR;
    }

    /* Mark the socket so it will listen for incoming connections */
    ret = listen(listenfd, server->backlog);
    if (ret == ERROR_SOCKET) {
        LOGGER_FATAL("listen() failed: %s", strerror(errno));
        goto APP_EXIT_ERROR;
    }

    /* run forever to wait client connections reached */
    LOGGER_INFO("%s-%s waiting clients...", FSYNC_SERVER_APP, VERSION);
    printf("* %s-%s waiting clients...\n", FSYNC_SERVER_APP, VERSION);

    for ( ; ; )  {
        clilen = sizeof(cliaddr);

        /* server blocks in the call to accept, waiting for
         *   a client connection to complete.
         */

        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
            if (errno == EAGAIN) {
                /* Resource temporarily unavailable */
                LOGGER_TRACE("accept() returns: [errno:%d] %s", errno, strerror(errno));
                continue;
            } else if (errno == EINTR) {
                /* Interrupted system call */
                LOGGER_TRACE("accept() returns: [errno:%d] %s", errno, strerror(errno));
                continue;
            } else {
                LOGGER_ERROR("accept() failed: [errno:%d] %s", errno, strerror(errno));
                break;
            }
        }

        if (SUCCESS != server_conf_setsocketopt(connfd, server)) {
            LOGGER_FATAL("should never run to this");
            break;
        }

        /* show connecting client ip and port */
        LOGGER_INFO("connecting client: %s:%d", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

        /**
         * authenticate client and create session if passed
         */

        num_childs++;


    }

    /* normally exit */
    server_conf_free(&server);

    LOGGER_INFO("%s-%s exit(0).", FSYNC_SERVER_APP, VERSION);
    LOGGER_FINI();

    printf("\n**** %s-%s exit(0).\n\n", FSYNC_SERVER_APP, VERSION);
    exit(0);

APP_EXIT_ERROR:
    /* error exit */
    server_conf_free(&server);

    if (listenfd != ERROR_SOCKET) {
        close(listenfd);
    }

    LOGGER_FATAL("%s-%s exit(-1).", FSYNC_SERVER_APP, VERSION);
    LOGGER_FINI();

    printf("\n**** %s-%s exit(-1).\n\n", FSYNC_SERVER_APP, VERSION);
    exit(-1);
}
