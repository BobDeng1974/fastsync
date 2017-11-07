/**
 * client.c
 *
 * Author:
 *     master@pepstack.com
 *
 * On client starts, it listens on inotify events. When a file creation event
 * reached, it then forks a child process - cliproc to communicate with server.
 *
 * cliproc creates a new socket connection with serproc spawned by server and
 * then synchronizes the file to the server.
 *
 * Usage:
 *   # fastsync-client -f ./conf/client-cfg.xml
 *
 * 2016-07-20: init created
 * 2017-02-01: last updated
 *
 */
#include "client.h"

#define EVENT_BUF_SIZE  4096


void exit_handler (int code, void * startcmd)
{
    sem_unlink(semaphore);
    printf ("\n* %s exit(%d).\n", FSYNC_CLIENT_APP, code);

    if (startcmd) {
        if (code == 10 || code == 11) {
            time_t t = time(0);
            //printf("\n\n* %s* isynclog-client restart: %s\n\n", ctime(&t), (char*) startcmd);
            code = pox_system(startcmd);
        }

        free(startcmd);
    }
}


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

    /* must call waitpid() */
    while ( (pid = waitpid (-1, &stat, WNOHANG)) > 0) {
        /* child process terminated */
    }
}


/**
 * kill -15 pid
 */
void sig_term (int signo)
{
    void pr_cpu_time(void);

    /* print cpu time */
    pr_cpu_time ();

    exit(signo);
}


/**
 * kill -2 pid
 */
void sig_int (int signo)
{
    void pr_cpu_time(void);

    /* print cpu time */
    pr_cpu_time();

    exit(signo);
}


#ifndef HAVE_GETRUSAGE_PROTO
int getrusage (int, struct rusage *);
#endif

void pr_cpu_time (void)
{
    double user, sys;
    struct rusage myusage, childusage;

    if (getrusage (RUSAGE_SELF, &myusage) < 0) {
        printf("\ngetrusage(RUSAGE_SELF) error.\n");
    }

    if (getrusage (RUSAGE_CHILDREN, &childusage) < 0) {
        printf("\ngetrusage(RUSAGE_CHILDREN) error.\n");
    }

    user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec / 1000000.0;
    user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / 1000000.0;
    sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec / 1000000.0;
    sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec / 1000000.0;

    printf("\n* user-time = %g, sys-time = %g\n", user, sys);
}


void run_forever(char * event_buf, size_t buf_size);


/**
 * main function
 */
int main(int argc, char ** argv)
{
    int len, handled;

    int isdaemon = FALSE;
    int verbose = FALSE;

    args_t  command_args;

    char * start_cmd = 0;
    time_t start_time = time(0);

    char event_buf[EVENT_BUF_SIZE]__attribute__((aligned(4)));

    /* command arguments */
    const struct option lopts[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {"verbose", no_argument, 0, 'v'},
        {"config", required_argument, 0, 'C'},
        {"log4crc", required_argument, 0, 'O'},
        {"priority", required_argument, 0, 'P'},
        {"appender", required_argument, 0, 'A'},
        {"daemon", no_argument, 0, 'D'},
        {"kill", no_argument, 0, 'K'},
        {"list", no_argument, 0, 'L'},
        {"md5", required_argument, 0, 'm'},
        {"regexp", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    void sig_chld(int);
    void sig_int(int);
    void sig_term(int);

    set_default_config(cfgfile, sizeof(cfgfile), log4crc, sizeof(log4crc));

    assert(! priority[0]);
    assert(! appender[0]);

    snprintf(event_buf, sizeof(event_buf), "%s", argv[0]);
    if (fullpath(event_buf, sizeof(event_buf)) != 0) {
        fprintf(stderr, "\033[31m[ERROR] fullpath failed for arg: '%s'\033[0m\n\n", argv[0]);
        exit(-1);
    }
    args_init(&command_args, 20);
    args_add(&command_args, event_buf);

    /* parse command arguments */
    while ((handled = getopt_long(argc, argv, "DKLhVvC:O:P:A:m:r:", lopts, 0)) != EOF) {
        switch (handled) {
        case 'D':
            isdaemon = TRUE;
            args_add(&command_args, "-D");
            break;

        case 'h':
            print_usage();
            args_fini(&command_args);
            exit(0);
            break;

        case 'C':
            /* overwrite default config file */
            snprintf(cfgfile, sizeof(cfgfile), "%s", optarg);
            break;

        case 'O':
            /* overwrite default log4crc file */
            snprintf(log4crc, sizeof(log4crc), "LOG4C_RCPATH=%s", optarg);
            break;

        case 'P':
            snprintf(priority, sizeof(priority), "%s", optarg);
            args_add(&command_args, "-P");
            args_add(&command_args, priority);
            break;

        case 'A':
            snprintf(appender, sizeof(appender), "%s", optarg);
            args_add(&command_args, "-A");
            args_add(&command_args, appender);
            break;

        case 'K':
            fprintf(stdout, "\033[35mkill all processes for program:\033[0m %s-%s\n", FSYNC_CLIENT_APP, VERSION);
            snprintf(event_buf, sizeof(event_buf), "%s-%s", FSYNC_CLIENT_APP, VERSION);
            len = find_pid_by_name(event_buf, kill_pid);
            if (len > 0) {
                fprintf(stdout, "* Total %d process(es) killed.\n\n", len);
            } else {
                fprintf(stdout, "* No process found, 0 killed.\n\n");
            }
            args_fini(&command_args);
            exit(0);
            break;

        case 'L':
            fprintf(stdout, "\033[35mlist of pids for program:\033[0m %s-%s\n", FSYNC_CLIENT_APP, VERSION);
            snprintf(event_buf, sizeof(event_buf), "%s-%s", FSYNC_CLIENT_APP, VERSION);
            len = find_pid_by_name(event_buf, list_pid);
            if (len > 0) {
                fprintf(stdout, "* Total %d process(es) found.\n\n", len);
            } else {
                fprintf(stdout, "* No process found.\n\n");
            }
            args_fini(&command_args);
            exit(0);
            break;

        case 'm':
            fprintf(stdout, "\033[35mmd5sum on file:\033[0m %s\n", optarg);
            if (check_file_error(optarg, R_OK) != 0) {
                fprintf(stderr, "\033[31m[error] file not found.\033[0m\n\n");
            } else {
                md5file(optarg);
            }
            args_fini(&command_args);
            exit(0);
            break;

        case 'r':
            if (argc > 1) {
                regexp(optarg, argv[1]);
            } else {
                fprintf(stderr, "\033[31mexpression required before -r.\033[0m\n\n");
            }
            args_fini(&command_args);
            exit(0);
            break;

        case 'V':
            fprintf(stdout, "\033[35m%s-%s, Build: %s %s\033[0m\n\n", FSYNC_CLIENT_APP, VERSION, __DATE__, __TIME__);
            args_fini(&command_args);
            exit(0);
            break;

        case 'v':
            verbose = TRUE;
            args_add(&command_args, "-v");
            break;
        }
    }

    /* config */
    snprintf(event_buf, sizeof(event_buf), "%s", cfgfile);
    if (fullpath(event_buf, sizeof(event_buf)) != 0) {
        fprintf(stderr, "\033[31m[ERROR] fullpath failed for cfgfile: '%s'\033[0m\n\n", cfgfile);
        args_fini(&command_args);
        exit(-1);
    }
    if (isdir(event_buf)) {
        snprintf(cfgfile, sizeof(cfgfile), "%s/client-cfg.xml", event_buf);
    } else {
        snprintf(cfgfile, sizeof(cfgfile), "%s", event_buf);
    }
    args_add(&command_args, "-C");
    args_add(&command_args, cfgfile);

    /* log4crc */
    snprintf(event_buf, sizeof(event_buf), "%s", &log4crc[13]);
    if (fullpath(event_buf, sizeof(event_buf)) != 0) {
        fprintf(stderr, "\033[31m[ERROR] fullpath failed for log4crc: '%s'\033[0m\n\n", &log4crc[13]);
        args_fini(&command_args);
        exit(-1);
    }
    snprintf(&log4crc[13], sizeof(log4crc) - 13, "%s", event_buf);
    args_add(&command_args, "-O");
    args_add(&command_args, &log4crc[13]);

    if (check_file_error(cfgfile, F_OK|R_OK|W_OK) != 0) {
        fprintf(stderr, "\033[31m[ERROR] Config File not found: '%s'\033[0m\n\n", cfgfile);
        args_fini(&command_args);
        exit(ERROR);
    }

    print_info(cfgfile, log4crc, start_time, isdaemon);

    /* check log4crc */
    update_log4c(FSYNC_CLIENT_APP, log4crc, priority, appender);

    LOGGER_INIT();

    if (0 != check_semphore(semaphore)) {
        snprintf(event_buf, sizeof(event_buf), "%s-%s", FSYNC_CLIENT_APP, VERSION);
        len = find_pid_by_name(event_buf, list_pid);
        if (len > 0) {
            LOGGER_ERROR("%s is already running.", FSYNC_CLIENT_APP);
        } else {
            LOGGER_ERROR("please remove semaphore file '/dev/shm/sem.%s' and retry!\n", semaphore);
        }
        args_fini(&command_args);
        exit(ERROR);
    }

    len = get_startcmd(command_args.argc, command_args.argv, event_buf, sizeof(event_buf));

    args_fini(&command_args);

    if (len == ERROR) {
        LOGGER_ERROR("get_startcmd failed");
        exit(ERROR);
    }
    start_cmd = malloc(strlen(event_buf) + 1);
    strcpy(start_cmd, event_buf);

    LOGGER_INFO("startcmd: [%s]", start_cmd);

    if (isdaemon) {
        /* runs in background */
        handled = daemon(0, 0);
        if (handled != 0) {
            perror("daemon");
            LOGGER_ERROR("daemon error(%d): %s", errno, strerror(errno));
            mem_free((void**) &start_cmd);
            exit(ERROR);
        } else {
            LOGGER_INFO("%s-%s daemon pid: %d", FSYNC_CLIENT_APP, VERSION, getpid());
        }
    }

    on_exit(exit_handler, start_cmd);

    /* SIGCHLD */
    if (signal(SIGCHLD, sig_chld) == SIG_ERR) {
        LOGGER_FATAL("signal(SIGCHLD) failed: %s", strerror(errno));
        exit(ERROR);
    }

    if (signal (SIGINT, sig_int) == SIG_ERR) {
        LOGGER_FATAL("signal(SIGINT) failed: %s", strerror(errno));
        exit(ERROR);
    }

    if (signal (SIGTERM, sig_term) == SIG_ERR) {
        LOGGER_FATAL("signal(SIGTERM) failed: %s", strerror(errno));
        exit(ERROR);
    }

    /* Unix supports the principle of piping, which allows processes to send data
     * to other processes without the need for creating temporary files. When a
     * pipe is broken, the process writing to it is sent the SIGPIPE signal.
     * The default reaction to this signal for a process is to terminate.
     */
    signal(SIGPIPE, SIG_IGN);

    run_forever(event_buf, sizeof(event_buf));

exit_onerror:

    LOGGER_ERROR("%s exit.", FSYNC_CLIENT_APP);
    LOGGER_FINI();

    pr_cpu_time();

    exit(-2);
}


void run_forever(char * event_buf, size_t buf_size)
{
    int at, len, handled;

    fd_set set;

    struct timeval timeout;
    struct inotify_event * event;

    client_conf_t * client;

    pthread_t  sweep_thread_id = 0;

    handled = client_create(cfgfile, &client);
    if (handled != SUCCESS) {
        LOGGER_ERROR("client_create failed");
        return;
    }

    if (0 != start_sweep_thread(&sweep_thread_id, client)) {
        goto exit_onerror;
    }

    for ( ; ; ) {
        FD_ZERO(&set);
        FD_SET(client->infd, &set);

        timeout.tv_sec = client->sweepinterval;
        timeout.tv_usec = 0;

        if (select(client->infd + 1, &set, 0, 0, &timeout) > 0) {
            if (FD_ISSET(client->infd, &set)) {
                handled = 0;

                while ((len = read(client->infd, &event_buf, buf_size)) > 0) {

                    at = 0;
                    handled = 0;

                    while (at < len) {

                        /* here we get an event */
                        event = (struct inotify_event *) (event_buf + at);

                        if (handled == -6 /* IN_Q_OVERFLOW */) {
                            /* IN_Q_OVERFLOW: inotify is overflow, do getting rid of overflow in queue. */
                            select_sleep(0, 1);
                        } else {
                            /* handle the event */
                            handled = handle_event(event, client);
                        }

                        /* move to next event */
                        at += (int) (sizeof(struct inotify_event) + event->len);
                    }
                }
            }
        } else {
            pthread_mutex_lock(&client->__lock);
            pthread_cond_signal(&client->condition);
            pthread_mutex_unlock(&client->__lock);
        }

        break;
    }

exit_onerror:

    if (sweep_thread_id) {
        LOGGER_TRACE("pthread_join: thread=%d", (int) sweep_thread_id);
        pthread_join(sweep_thread_id, 0);
    }

    LOGGER_TRACE("client_release");
    client_release(&client);
}

