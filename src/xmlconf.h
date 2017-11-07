/**
* xmlconf.h
*
* 2016-07-06: init created
*/
#ifndef XML_CONF_H_INCLUDED
#define XML_CONF_H_INCLUDED

#include <mxml.h>

#include "msgs.h"


#define CHECK_NODE_ERR(node, errnum) \
    if (!node) { \
        err = (errnum); \
        goto ERROR_RET; \
    }


static int safe_stricmp(const char *a, const char *b)
{
    if (!a || !b) {
        return -1;
    }
    else {
        int ca, cb;
        do {
            ca = (unsigned char) *a++;
            cb = (unsigned char) *b++;
            ca = tolower(toupper(ca));
            cb = tolower(toupper(cb));
        } while (ca == cb && ca != '\0');

        return ca - cb;
    }
}


static int parse_bool_asint(const char * boolValue, int *intValue)
{
    char value[10];
    int tolen;

    *intValue = 0;

    tolen = snprintf(value, sizeof(value), "%s", boolValue);

    if (tolen > 0 && tolen < sizeof(value)) {
        const char * true_array[] = {"1", "yes", "y", "true", "t", "enabled", "enable", "valid", 0};
        const char * false_array[] = {"0", "no", "n", "false", "f", "disabled", "disable", "invalid", 0};

        strlwr(value);

        if (str_in_array(value, true_array)) {
            *intValue = TRUE;
            return TRUE;
        } else if (str_in_array(value, false_array)) {
            *intValue = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}


static int parse_second_ratio(char * timeuint, float *second_ratio)
{
    float sr = 1.0f;

    const char * ms_array[] = {"millisecond", "msec", "ms", 0};
    const char * s_array[] = {"default", "second", "sec", "s", "", 0};
    const char * m_array[] = {"minute", "min", "m", 0};
    const char * h_array[] = {"hour", "hr", "h", 0};

    strlwr(timeuint);

    if (str_in_array(timeuint, s_array)) {
        sr = 1.0f;
    } else if (str_in_array(timeuint, ms_array)) {
        sr = 0.001f;
    } else if (str_in_array(timeuint, m_array)) {
        sr = 60.0f;
    } else if (str_in_array(timeuint, h_array)) {
        sr = 3600.0f;
    } else {
        return FALSE;
    }

    *second_ratio = sr;
    return TRUE;
}


static int parse_byte_ratio(char * sizeuint, float *byte_ratio)
{
    float br;

    const char * mb_array[] = {"megabyte", "mbyte", "mb", "m", 0};
    const char * kb_array[] = {"kilobyte", "kbyte", "kb", "k", 0};
    const char * gb_array[] = {"gigabyte", "gbyte", "gb", "g", 0};
    const char * b_array[] = {"default", "byte", "b", "", 0};

    strlwr(sizeuint);

    if (str_in_array(sizeuint, b_array)) {
        br = 1.0f;
    } else if (str_in_array(sizeuint, kb_array)) {
        br = 1024;
    } else if (str_in_array(sizeuint, mb_array)) {
        br = 1024*1024;
    } else if (str_in_array(sizeuint, gb_array)) {
        br = 1024*1024*1024;
    } else {
        return FALSE;
    }

    *byte_ratio = br;
    return TRUE;
}


static int MxmlNodeGetStringAttr(mxml_node_t *node, const char * nodeName, char * strValue, int sizeValue)
{
    const char * szAttrTemp;
    int tolen;

    *strValue = 0;

    if (! node) {
        return FALSE;
    }

    szAttrTemp = mxmlElementGetAttr(node, nodeName);
    if (! szAttrTemp) {
        return FALSE;
    }

    tolen = snprintf(strValue, sizeValue, "%s", szAttrTemp);
    if (tolen > 0 && tolen < sizeValue) {
        return TRUE;
    }

    return FALSE;
}


static int MxmlNodeGetIntegerAttr(mxml_node_t *node, const char * nodeName, int * intValue)
{
    const char * szAttrTemp;

    *intValue = 0;

    if (! node) {
        return FALSE;
    }

    szAttrTemp = mxmlElementGetAttr(node, nodeName);
    if (! szAttrTemp || ! strcmp(szAttrTemp, "") || ! strcmp(szAttrTemp, "(null)")) {
        return FALSE;
    } else if (parse_bool_asint(szAttrTemp, intValue)) {
        return TRUE;
    }

    *intValue = atoi(szAttrTemp);
    return TRUE;
}


typedef int (*ListNodeCallback)(mxml_node_t *, void *);

static int MxmlNodeListChildNodes(mxml_node_t *parentNode, const char * childNodeName, ListNodeCallback nodeCallback, void *param)
{
    mxml_node_t * childNode;

    for (childNode = mxmlFindElement(parentNode, parentNode, childNodeName, 0, 0, MXML_DESCEND);
        childNode != 0;
        childNode = mxmlFindElement(childNode, parentNode, childNodeName, 0, 0, MXML_DESCEND)) {

        if (! nodeCallback(childNode, param)) {
            return FALSE;
        }
    }

    return TRUE;
}


#endif /* XML_CONF_H_INCLUDED */
