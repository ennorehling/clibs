/*
Copyright (c) 2010-2015, Enno Rehling <enno@eressea.de>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**/

#ifndef _SELIST_H
#define _SELIST_H

/* From http://en.wikipedia.org/wiki/Unrolled_linked_list
 * In computer programming, an unrolled linked list is a variation on the
 * linked list which stores multiple elements in each node. It can
 * dramatically increase cache performance, while decreasing the memory
 * overhead associated with storing list metadata such as references. It
 * is related to the B-tree.
*/

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct selist selist;
    typedef void (*selist_cb)(void *entry);
    typedef bool (*selist_cbex)(void *entry, void *more);

    void * selist_get(const struct selist *ql, int qi);
    int selist_length(const struct selist *ql);
    void selist_free(struct selist *ql);
    int selist_empty(const struct selist *ql);

    struct selist * selist_push(struct selist **qlp, void *data);
    int selist_delete(struct selist **qlp, int index);
    bool selist_find(struct selist **qlp, int *index, const void *value, int(*equal)(const void *, const void *));
    bool selist_contains(selist *ql, const void *value, int(*equal)(const void *, const void *));
    int selist_insert(struct selist **qlp, int index, void *data);
    void selist_foreach(struct selist *ql, selist_cb cb);
    void selist_foreach_ex(struct selist *ql, selist_cbex cb, void *);
    int selist_advance(struct selist **iterator, int *index, int stride);
    void *selist_replace(struct selist *ql, int index, void *data);
    void selist_map_reduce(struct selist *ql, void (*mapfunc)(void *entry, void *data), void(*reducefunc)(void *data, void *result), void *data, void *result);

    typedef int(*cmp_cb)(const void *, const void *);
    bool selist_set_insert(struct selist **qlp, void *data, cmp_cb);
    bool selist_set_find(struct selist **qlp, int *qip, const void *data, cmp_cb);

#ifdef __cplusplus
}
#endif
#endif
