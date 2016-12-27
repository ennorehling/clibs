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

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct selist selist;
    typedef void (*selist_cb)(void *entry, void *more);

    void * selist_get(const struct selist *ql, int qi);
    int selist_length(const struct selist *ql);
    void selist_free(struct selist *ql);
    int selist_empty(const struct selist *ql);

    struct selist * selist_push(struct selist **qlp, void *data);
    int selist_delete(struct selist **qlp, int index);
    int selist_find(struct selist **qlp, int *index, const void *value, int(*equal)(const void *, const void *));
    int selist_insert(struct selist **qlp, int index, void *data);
    void selist_foreach(struct selist *ql, selist_cb cb, void *);
    int selist_advance(struct selist **iterator, int *index, int stride);
    void *selist_replace(struct selist *ql, int index, void *data);
    void selist_map_reduce(struct selist *ql, void (*mapfunc)(void *entry, void *data), void(*reducefunc)(void *data, void *result), void *data, void *result);

    /*
    typedef struct ql_iter {
        struct quicklist *l, **lp;
        int i;
    } ql_iter;
    ql_iter qli_init(struct quicklist **qlp);
    int qli_more(ql_iter iter);
    void * qli_get(ql_iter iter);
    void * qli_next(struct ql_iter *iter);
    void qli_delete(struct ql_iter *iter);
    
    int ql_set_insert(struct quicklist **qlp, void *data);
    int ql_set_insert_ex(struct quicklist **qlp, void *data, int (*cmp_cb)(const void *lhs, const void *rhs));
    int ql_set_find(struct quicklist **qlp, int *qip, const void *data);
    int ql_set_find_ex(struct quicklist **qlp, int *qip, const void *data, int (*cmp_cb)(const void *lhs, const void *rhs));
    int ql_set_remove(struct quicklist **qlp, const void *data);
*/

#ifdef __cplusplus
}
#endif
#endif