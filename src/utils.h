/**
* utils.h
*   common api
*
* Last Updated: 2017-01-09
* Last Updated: 2017-01-09
*/
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "heads.h"


static int select_sleep (int sec, int ms)
{
    if (sec || ms) {
        struct timeval tv = {0};

        tv.tv_sec = sec;
        tv.tv_usec = ms * 1000;

        return select (0, NULL, NULL, NULL, &tv);
    } else {
        return 0;
    }
}


/**
 * check_semphore
 *   Find existing one for given name.
 *
 * last: 2017-01-09
 */
static int check_semphore (const char * sem_name)
{
    sem_t * sem_p = sem_open(sem_name, O_CREAT|O_EXCL, 0644, 1);
    if (sem_p == SEM_FAILED) {
        perror(sem_name);
        return -1;
    }
    return 0;
}


static int isdir(char *path)
{
    struct stat sb;
    int rc;
    rc = stat(path, &sb);
    if (rc == 0 && (sb.st_mode & S_IFDIR)) {
        /* is dir */
        return 1;
    } else {
        /* not dir */
        return 0;
    }
}


static int fullpath (char * relpath, int sizepath)
{
    int rc;
    char *p;

    char filename[FILENAME_MAX];
    char cwd[PATH_MAX];

    *filename = 0;

    /* save current work dir */
    p = getcwd(cwd, sizeof(cwd));
    if (! p) {
        return (-1);
    }

    if (! isdir(relpath)) {
        p = strrchr(relpath, '/');
        *p++ = 0;
        snprintf(filename, sizeof(filename), "/%s", p);
    }

    rc = chdir(relpath);
    if (rc != 0) {
        return (-1);
    }

    p = getcwd(relpath, sizepath);
    if (! p) {
        rc = chdir(cwd);
        return (-1);
    }

    strcat(relpath, filename);

    /* restore current work dir */
    rc = chdir(cwd);
    if (rc != 0) {
        return (-1);
    }

    return 0;
}


/**
 * getpwd
 *   Get absolute path (end with '/') for current process.
 *
 * last: 2017-01-09
 */
static int getpwd (char *path, int size)
{
    ssize_t r;
    char * p;

    r = readlink("/proc/self/exe", path, size);
    if (r < 0) {
        strerror_r(errno, path, size);
        return 0;
    }

    if (r >= size) {
        snprintf(path, size, "insufficent buffer");
        return 0;
    }
    path[r] = '\0';

    p = strrchr(path, '/');
    if (! p) {
        snprintf(path, size, "invalid link");
        return 0;
    }
    *++p = 0;

    return p - path;
}


#define PREFIX_LOCAL_BIN    "/usr/local/bin"

static const char * get_bindir (char * cmd, char * bindir, int size)
{
    char * p = strrchr(cmd, '/');
    *bindir = 0;

    if (p) {
        /* get only abspath to dir */
        char dir[MAX_FILENAME_LEN + 1];
        strcpy(dir, cmd);
        dir[p - cmd] = 0;
        if (! realpath(dir, bindir)) {
            perror("realpath");
            return 0;
        }
    } else {
        /* is slink */
        snprintf(bindir, size, "%s", PREFIX_LOCAL_BIN);
    }

    bindir[size - 1] = 0;
    return bindir;
}


static const char* cmd_system(const char *cmdline, char *outbuf, ssize_t outsize)
{
    char * result = 0;

    FILE * fp = popen(cmdline, "r");
    if (fp) {
        bzero(outbuf, outsize);

        while (fgets(outbuf, outsize, fp) != 0) {
            result = outbuf;
        }

        pclose(fp);
    }

    return result;
}


typedef void (*sighandler_t)(int);

static int pox_system (const char *cmd_line)
{
    int ret = 0;
    sighandler_t old_handler;

    old_handler = signal (SIGCHLD, SIG_DFL);
    ret = system (cmd_line);
    if (ret != 0) {
        perror(cmd_line);
    }
    signal (SIGCHLD, old_handler);

    return ret;
}


typedef int (*find_pid_callback)(int, void*);

struct pid_func_info {
    int counter;
    int thispid;
    const char * path;
};


static int find_pid_by_name (const char* procname, find_pid_callback pid_func)
{
    DIR            *dir;
    struct dirent  *d;
    int            pid, i;
    char           *s;
    int            pnlen;

    struct pid_func_info  pfi = {0, getpid(), 0};

    i = 0;

    pnlen = strlen(procname);

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (! dir) {
        perror("opendir");
        return -1;
    }

    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) {
        char exe [PATH_MAX+1];
        char path[PATH_MAX+1];
        int len;
        int namelen;

        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0) {
            continue;
        }

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if ((len = readlink(exe, path, PATH_MAX)) < 0) {
            continue;
        }

        path[len] = '\0';

        /* find procname */
        s = strrchr(path, '/');
        if(s == NULL) {
            continue;
        }
        s++;

        /* we don't need small name len */
        namelen = strlen(s);
        if (namelen < pnlen) {
            continue;
        }

        if (!strncmp(procname, s, pnlen)) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                pfi.path = path;

                if (! pid_func(pid, (void*) & pfi)) {
                    break;
                }
            }
        }
    }

    closedir(dir);

    return  pfi.counter;
}


static int kill_pid (int pid, void * arg)
{
    struct pid_func_info * pfi = (struct pid_func_info *) arg;

    if (pid != pfi->thispid) {
        int ret = kill (pid, 0);
        if (ret) {
            perror("kill");
        } else {
            ret = kill(pid, SIGTERM);
            if (ret) {
                perror("kill");
            } else {
                fprintf(stdout, "<%d> process(%d) killed: %s\n", ++pfi->counter, pid, pfi->path);
            }
        }
    }

    return 1;
}


static int list_pid (int pid, void * arg)
{
    struct pid_func_info * pfi = (struct pid_func_info *) arg;

    if (pid != pfi->thispid) {
        fprintf(stdout, "<%d> process(%d) found: %s\n", ++pfi->counter, pid, pfi->path);
    }

    return 1;
}


static int check_file_error (const char * file, int mode /* R_OK, W_OK */)
{
    if (0 == access(file, mode)) {
        return 0;
    } else {
        perror(file);
        return -1;
    }
}


static void update_log4c(const char * catname, char * log4crc, char * new_priority, char * new_appender)
{
    int ret;

    struct stat sb;

    char result[400];
    char cmdline[400];

    char category[200];
    char new_category[200];

    char priority[30] = "info";
    char appender[30] = "stdout";

    const char * log4cfile = strchr(log4crc, '/');

    int endpos = strlen(log4crc);

    strcat(log4crc, "/log4crc");

    if (! log4cfile) {
        fprintf(stderr, "error log4crc: %s", log4crc);
        exit(ERROR);
    }

    if (stat(log4cfile, &sb) == -1 && errno == ENOENT) {
        perror(log4cfile);
        exit(ERROR);
    }

    if (access(log4cfile, F_OK|R_OK|W_OK) != 0) {
        perror(log4cfile);
        exit(ERROR);
    }

    /* get old priority from log4crc */
    snprintf(cmdline, sizeof(cmdline),
        "grep '<category name=\"%s\" priority=\"' '%s' | sed -r 's/.* priority=\"(.*)\" appender=\".*/\\1/'",
        catname, log4cfile);

    if (cmd_system(cmdline, result, sizeof(result))) {
        snprintf(priority, sizeof(priority), "%s", trims(result, " \n"));
    }

    /* get old appender from log4crc */
    snprintf(cmdline, sizeof(cmdline),
        "grep '<category name=\"%s\" priority=\"' '%s' | sed -r 's/.* appender=\"(.*)\" .*/\\1/'",
        catname, log4cfile);

    if (cmd_system(cmdline, result, sizeof(result))) {
        snprintf(appender, sizeof(appender), "%s", trims(result, " \n"));
    }

    /* make up old category */
    snprintf(category, sizeof(category),
        "<category name=\"%s\" priority=\"%s\" appender=\"%s\" \\/>",
        catname, priority, appender);

    /* update log4crc with priority and appender */
    if (*new_priority) {
        const char * priority_array[] = {"fatal", "error", "warn", "info", "debug", "trace", 0};

        if (! str_in_array(strlwr(new_priority), priority_array)) {
            fprintf(stderr, "unknown priority: %s\n", new_priority);
            exit(ERROR);
        } else {
            snprintf(priority, sizeof(priority), "%s", new_priority);
        }
    }

    if (*new_appender) {
        const char * appender_array[] = {"default", "stdout", "stderr", "syslog", 0};

        if (! str_in_array(strlwr(new_appender), appender_array)) {
            fprintf(stderr, "unknown appender: %s\n", new_appender);
            exit(ERROR);
        } else if (! strcmp(new_appender, "default")) {
            snprintf(appender, sizeof(appender), "%s-appender", catname);
        } else {
            snprintf(appender, sizeof(appender), "%s", new_appender);
        }
    }

    /* make up new category */
    snprintf(new_category, sizeof(new_category),
        "<category name=\"%s\" priority=\"%s\" appender=\"%s\" \\/>",
        catname, priority, appender);

    if (strcmp(category, new_category)) {
        snprintf(cmdline, sizeof(cmdline), "sed -i 's/%s/%s/g' '%s'", category, new_category, log4cfile);
        ret = pox_system(cmdline);
        if (ret != 0) {
            exit(ret);
        }
    }

    if (! strcmp(new_appender, "default")) {
        /* get logdir from log4crc */
        snprintf(cmdline, sizeof(cmdline),
            "grep '<appender name=\"%s-appender\" logdir=\"' '%s' | sed -r 's/.* logdir=\"(.*)\".*/\\1/'",
            catname, log4cfile);

        if (cmd_system(cmdline, result, sizeof(result))) {
            snprintf(cmdline, sizeof(cmdline), "%s", trims(result, " \n"));
        }

        fprintf(stdout, "* log4c_logdir: \033[47;35m%s\033[0m\n", cmdline);

        if (stat(cmdline, &sb) == -1 && errno == ENOENT) {
            perror(cmdline);
            exit(ERROR);
        }

        if (access(cmdline, F_OK|R_OK|W_OK) != 0) {
            perror(cmdline);
            exit(ERROR);
        }
    }

    fprintf(stdout, "* log4c_rcfile: \033[47;35m%s\033[0m\n", log4cfile);
    fprintf(stdout, "* log4c_priority: \033[47;35m%s\033[0m\n", priority);
    fprintf(stdout, "* log4c_appender: \033[47;35m%s\033[0m\n", appender);

    log4crc[endpos] = 0;
    if (0 != putenv(log4crc)) {
        perror(log4crc);
        exit(ERROR);
    }
}


#endif /* UTILS_H_INCLUDED */
