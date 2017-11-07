/**
* args.h
*   command arguments parser api
*
* Last Updated: 2017-01-25
* Last Updated: 2017-01-25
*/
#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct args_t
{
    int argc;
    char **argv;
} args_t;


static void args_init(args_t * args, int maxargc)
{
    args->argc = 0;
    args->argv = (char**) calloc(maxargc, sizeof(char*));
}


static void args_fini(args_t * args)
{
    int i = 0;
    for (; i < args->argc; ++i) {
        free(args->argv[i]);
    }
    free(args->argv);
    args->argc = 0;
}


static void args_add(args_t * args, const char * arg)
{
    int size = strlen(arg) + 1;
    args->argv[args->argc] = calloc(size + 1, sizeof(char));
    memcpy(args->argv[args->argc], arg, size);
    args->argc++;
}


static void args_print(args_t * args)
{
    int i = 0;
    for (; i < args->argc; ++i) {
        printf("argv[%d]=%s\n", i, args->argv[i]);
    }
}


#endif /* ARGS_H_INCLUDED */

