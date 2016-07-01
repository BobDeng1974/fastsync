/**
* logger.h
*
* Init Created: 2014-08-01
* Last Updated: 2014-08-01
*
* Linux time commands:
*   http://blog.sina.com.cn/s/blog_4171e80d01010rtr.html
*/
#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <log4c.h>

#ifndef LOGGER_BUF_LEN
    #define LOGGER_BUF_LEN  1020
#endif


#ifdef LOGGER_CATEGORY_NAME
    #define LOGGER_CATEGORY_NAME_REAL  LOGGER_CATEGORY_NAME
#else
    #define LOGGER_CATEGORY_NAME_REAL  "root"
#endif


static log4c_category_t * logger_get_cat (int priority)
{
    log4c_category_t * cat = log4c_category_get (LOGGER_CATEGORY_NAME_REAL);

    if (cat && log4c_category_is_priority_enabled (cat, priority)) {
        return cat;
    } else {
        return 0;
    }
}


static void logger_write (log4c_category_t *cat, int priority,
    const char *file, int line, const char *func, const char * msg)
{
    log4c_category_log (cat, priority, "(%s:%d) <%s> %s", file, line, func, msg);
}


static void LOGGER_INIT ()
{
    if (0 != log4c_init ()) {
        printf("\n**** log4c_init() failed.\n");
        printf("\n* This error might occur on Redhat Linux if you had installed expat before log4c.\n");
        printf("\n* To solve this issue you can uninstall both log4c and expat and reinstall them.\n");
        printf("\n* Please make sure log4c must be installed before expat installation on Redhat.\n");
        printf("\n* The other reasons cause failure are not included here, please refer to the manual.\n");
    } else {
        printf("\n* LOGGER_INIT(%s).\n", LOGGER_CATEGORY_NAME_REAL);
    }
}


static void LOGGER_FINI ()
{
    printf("\n* LOGGER_FINI()!\n");
    log4c_fini ();
}


#if defined(LOGGER_TRACE_UNKNOWN)
#   define LOGGER_UNKNOWN(msg, args...)    do {} while (0)
#else
#   define LOGGER_UNKNOWN(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_UNKNOWN); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_UNKNOWN, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#if defined(LOGGER_TRACE_DISABLED)
#   define LOGGER_TRACE(msg, args...)    do {} while (0)
#else
#   define LOGGER_TRACE(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_TRACE); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_TRACE, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#if defined(LOGGER_DEBUG_DISABLED)
#   define LOGGER_DEBUG(msg, args...)    do {} while (0)
#else
#   define LOGGER_DEBUG(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_DEBUG); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_DEBUG, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#if defined(LOGGER_WARN_DISABLED)
#   define LOGGER_WARN(msg, args...)    do {} while (0)
#else
#   define LOGGER_WARN(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_WARN); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_WARN, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#if defined(LOGGER_INFO_DISABLED)
#   define LOGGER_INFO(msg, args...)    do {} while (0)
#else
#   define LOGGER_INFO(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_INFO); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_INFO, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#if defined(LOGGER_ERROR_DISABLED)
#   define LOGGER_ERROR(msg, args...)    do {} while (0)
#else
#   define LOGGER_ERROR(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_ERROR); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_ERROR, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#if defined(LOGGER_FATAL_DISABLED)
#   define LOGGER_FATAL(msg, args...)    do {} while (0)
#else
#   define LOGGER_FATAL(msg, args...)    \
    do { \
        log4c_category_t * cat = logger_get_cat (LOG4C_PRIORITY_FATAL); \
        if (cat) { \
            char tmp[LOGGER_BUF_LEN+1]; \
            snprintf (tmp, LOGGER_BUF_LEN, msg, ##args); \
            tmp[LOGGER_BUF_LEN] = 0; \
            logger_write (cat, LOG4C_PRIORITY_FATAL, __FILE__, __LINE__, __FUNCTION__, tmp); \
        } \
    } while (0)
#endif


#endif /* LOGGER_H_INCLUDED */
