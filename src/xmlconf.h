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


    
static const char * bool_trues[] = {
    "true",
    "t",
    "yes",
    "y",
    "1",
    0
};


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


static int bool_to_int_0or1(const char *bval)
{
    if (!bval || !strcmp(bval, "") || !strcmp(bval, "(null)")) {
        return 0;
    } else {
        const char ** bbv = bool_trues;
        while (*bbv) {
            if (! safe_stricmp(bval, *bbv)) {
                return 1;
            }
            bbv++;
        }
        return 0;
    }
}


static int MxmlNodeGetStringAttr(mxml_node_t *node, const char * nodeName, char * strValue, int sizeValue)
{
    const char * szAttrTemp;

    if (!node) {
        *strValue = 0;
        return 0;
    }

    szAttrTemp = mxmlElementGetAttr(node, nodeName);
    if (!szAttrTemp) {
        *strValue = 0;
        return 0;
    }

    strncpy(strValue, szAttrTemp, sizeValue);
    strValue[sizeValue - 1] = 0;
    return 1;
}


static int MxmlNodeGetIntegerAttr(mxml_node_t *node, const char * nodeName, int * intValue)
{
    const char * szAttrTemp;

    if (!node) {
        *intValue = 0;
        return 0;
    }

    szAttrTemp = mxmlElementGetAttr(node, nodeName);
    if (!szAttrTemp || !strcmp(szAttrTemp, "") || !strcmp(szAttrTemp, "(null)")) {
        *intValue = 0;
        return 0;
    }

    if (
        ! safe_stricmp(szAttrTemp, "true") ||
        ! safe_stricmp(szAttrTemp, "t") ||
        ! safe_stricmp(szAttrTemp, "yes") ||
        ! safe_stricmp(szAttrTemp, "y")) {
        *intValue = 1;
    } else if (
        ! safe_stricmp(szAttrTemp, "false") ||
        ! safe_stricmp(szAttrTemp, "f") ||
        ! safe_stricmp(szAttrTemp, "no") ||
        ! safe_stricmp(szAttrTemp, "n")) {
        *intValue = 0;
    } else {
        *intValue = atoi(szAttrTemp);
    }

    return 1;
}


#endif /* XML_CONF_H_INCLUDED */
