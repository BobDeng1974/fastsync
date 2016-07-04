/**
* mem.h
*
* 2014-08-01: init created
*/
#ifndef MEM_H_INCLUDED
#define MEM_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * include stdlib before jemalloc
 * link: LDFLAGS ?= /usr/local/lib/libjemalloc.a -lpthread
 */
#include <stdlib.h>
#include <jemalloc/jemalloc.h>

static void * mem_alloc (int num, size_t size)
{
    void * pv = calloc (num, size);
    if (! pv) {
        perror("mem_alloc");
        exit (-1);
    }
    return pv;
}


static void mem_free (void **ppv)
{
    if (ppv) {
        void * pv = * ppv;
        if (pv) {
            free (pv);
        }
        *ppv = 0;
    }
}


/**
* The compiler tries to warn you that you lose bits when casting from void *
*   to int. It doesn't know that the void * is actually an int cast, so the
*   lost bits are meaningless.
*
* A double cast would solve this (int)(uintptr_t)t->key. It first casts void *
*   to uintptr_t (same size, no warning), then uintptr_t to int (number to
*   number, no warning). Need to include <stdint.h> to have the uintptr_t type
*   (an integral type with the same size as a pointer).
*/
#define pv_cast_to_int(pv)    ((int) (uintptr_t) (void*) (pv))
#define int_cast_to_pv(ival)    ((void*) (uintptr_t) (int) (ival))


#endif /* MEM_H_INCLUDED */
