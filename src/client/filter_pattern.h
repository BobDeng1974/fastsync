/**
 * filter_pattern.h
 *
 * Author:
 *     master@pepstack.com
 *
 * 2017-01-14: init created
 * 2017-01-14: last updated
 *
 */
#ifndef FILTER_PATTERN_H_INCLUDED
#define FILTER_PATTERN_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#include "../fsyncdef.h"

#define LOGGER_COLOR_OUTPUT
#define LOGGER_CATEGORY_NAME  FSYNC_CLIENT_APP
#include "../logger.h"


/**
 * filter_pattern_t
 */
typedef struct filter_pattern_t
{
    int patterns;
    char **pattern_list;
} *filter_pattern, filter_pattern_t;


static void filter_pattern_add(filter_pattern fp, const char *str)
{
    char * psz;
    int len = strlen(str);

    CHECK_MEMORY_CAPACITY(fp->pattern_list, char*, fp->patterns);

    psz = malloc(len + 1);
    if (! psz) {
        exit(EMEMORY);
    }

    memcpy(psz, str, len);
    psz[len] = '\0';

    fp->pattern_list[fp->patterns++] = psz;
}


static void filter_pattern_free(filter_pattern fp)
{
    int i;
    char * psz;

    for (i = 0; i < fp->patterns; ++i) {
        psz = fp->pattern_list[i];
        fp->pattern_list[i] = 0;
        free(psz);
    }

    fp->patterns = 0;

    free(fp->pattern_list);
}


static void filter_pattern_print(const filter_pattern_t * fp, const char * pattern)
{
    int i;

    for (i = 0; i < fp->patterns; ++i) {
        LOGGER_INFO("%s: %s", pattern, fp->pattern_list[i]);
    }
}


#if defined(__cplusplus)
}
#endif

#endif /* FILTER_PATTERN_H_INCLUDED */
