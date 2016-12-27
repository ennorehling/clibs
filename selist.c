﻿/*
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

#include "selist.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define LIST_MAXSIZE 14 /* max. number of elements unrolled into one node */
#define LIST_LIMIT 7 /* this many or fewer number in a node => attempt merge */

/* The total size of this struct is 64 bytes on a 32-bit system with
 * normal alignment. YMMV, so on a 64-bit system, twiddle the
 * constants above */
struct selist {
    struct selist *next;
    int num_elements;
    void *elements[LIST_MAXSIZE];
};

int selist_find(struct selist **qlp, int *index, const void *value, int(*match)(const void *, const void *))
{
    for (; *qlp; selist_advance(qlp, index, 1)) {
        void *x = selist_get(*qlp, *index);
        if (match ? match(value, x) : value == x) {
            return 1;
        }
    }
    return 0;
}

void *selist_get(const selist * ql, int i)
{
    assert(ql);
    return (i < ql->num_elements) ? ql->elements[i] : selist_get(ql->next, i - ql->num_elements);
}

void *selist_replace(selist * ql, int i, void *data)
{
    assert(ql);
    if (i < ql->num_elements) {
        void *orig = ql->elements[i];
        ql->elements[i] = data;
        return orig;
    }
    else {
        return selist_replace(ql->next, i - ql->num_elements, data);
    }
}

int selist_length(const selist * ql)
{
    return ql ? ql->num_elements + selist_length(ql->next) : 0;
}

int selist_empty(const selist * ql)
{
    return !ql;
}

selist * selist_push(selist ** qlp, void *data)
{
    selist *ql = 0;
    while (*qlp && ((*qlp)->next || (*qlp)->num_elements == LIST_MAXSIZE)) {
        qlp = &(*qlp)->next;
    }
    if (!*qlp) {
        ql = (selist *)malloc(sizeof(selist));
        if (ql) {
            ql->num_elements = 0;
            ql->next = 0;
            *qlp = ql;
        }
    }
    else {
        ql = *qlp;
    }
    if (ql) {
        ql->elements[ql->num_elements++] = data;
    }
    return ql;
}

int selist_delete(selist ** qlp, int i)
{
    selist *ql = *qlp;
    if (i < 0)
        return EINVAL;
    if (ql && i >= ql->num_elements) {
        return selist_delete(&ql->next, i - ql->num_elements);
    }
    else if (ql) {
        if (i + 1 < ql->num_elements) {
            memmove(ql->elements + i, ql->elements + i + 1,
                (ql->num_elements - i - 1) * sizeof(void *));
        }
        --ql->num_elements;
        if (ql->num_elements == 0) {
            *qlp = ql->next;
            free(ql);
        }
        else if (ql->next && ql->num_elements <= LIST_LIMIT) {
            selist *qn = ql->next;
            if (ql->num_elements + qn->num_elements > LIST_MAXSIZE) {
                ql->elements[ql->num_elements] = qn->elements[0];
                --qn->num_elements;
                ++ql->num_elements;
                memmove(qn->elements, qn->elements + 1,
                    qn->num_elements * sizeof(void *));
            }
            else {
                memcpy(ql->elements + ql->num_elements, qn->elements,
                    qn->num_elements * sizeof(void *));
                ql->num_elements += qn->num_elements;
                ql->next = qn->next;
                free(qn);
            }
        }
    }
    return 0;
}

int selist_insert(selist ** qlp, int i, void *data)
{
    selist *ql = *qlp;
    if (ql) {
        if (i >= LIST_MAXSIZE) {
            return selist_insert(&ql->next, i - ql->num_elements, data);
        }
        else if (ql->num_elements < LIST_MAXSIZE && i<=ql->num_elements) {
            if (i<ql->num_elements) {
                memmove(ql->elements + i + 1, ql->elements + i,
                        (ql->num_elements - i) * sizeof(void *));
            }
            ql->elements[i] = data;
            ++ql->num_elements;
        }
        else {
            selist *qn = (selist *)malloc(sizeof(selist));
            if (qn) {
                qn->next = ql->next;
                ql->next = qn;
                qn->num_elements = ql->num_elements - LIST_LIMIT;
                ql->num_elements = LIST_LIMIT;
                memcpy(qn->elements, ql->elements + ql->num_elements, //TODO: V512 http://www.viva64.com/en/V512 A call of the 'memcpy' function will lead to underflow of the buffer 'qn->elements'.
                    LIST_LIMIT * sizeof(void *));
            }
            if (i <= ql->num_elements) {
                return selist_insert(qlp, i, data);
            }
            else {
                return selist_insert(&ql->next, i - ql->num_elements, data);
            }
        }
    }
    else if (i == 0) {
        selist_push(qlp, data);
    }
    else {
        return EINVAL;
    }
    return 0;
}

void selist_foreach(struct selist *ql, selist_cb cb, void *more)
{
    for (; ql; ql = ql->next) {
        int i;
        for (i = 0; i != ql->num_elements; ++i) {
            cb(ql->elements[i], more);
        }
    }
}

int selist_advance(struct selist **iterator, int *index, int stride)
{
    selist *ql = *iterator;
    int i = *index;
    if (!ql || i < 0 || stride < 0) {
        return ERANGE;
    }
    else if (i + stride < ql->num_elements) {
        *index = i + stride;
        return 0;
    }
    else {
        *index = i - ql->num_elements + stride;
        *iterator = ql->next;
        return selist_advance(iterator, index, 0);
    }
}

void selist_free(struct selist *ql)
{
    while (ql) {
        selist * qn = ql;
        ql = ql->next;
        free(qn);
    }
}

void selist_map_reduce(struct selist *ql, void(*mapfunc)(void *entry, void *data), void(*reducefunc)(void *data, void *result), void *data, void *result)
{
    int qi;
    for (qi = 0; ql; selist_advance(&ql, &qi, 1)) {
        mapfunc(selist_get(ql, qi), data);
        reducefunc(data, result);
    }
}

#if 0

int selist_set_remove(struct selist **qlp, const void *data)
{
    int qi;
    selist *ql = *qlp;

    if (!ql)
        return 0;

    for (qi = 0; qi != ql->num_elements; ++qi) {
        void *qd = selist_get(ql, qi);
        if (qd == data) {
            return selist_delete(qlp, qi) == 0;
        }
    }
    return selist_set_remove(&ql->next, data);
}

int selist_set_insert(struct selist **qlp, void *data)
{
    if (*qlp) {
        selist *ql = *qlp;
        if (ql->num_elements > 0 && ql->elements[ql->num_elements - 1] < data) {
            if (ql->num_elements == LIST_MAXSIZE || (ql->next
                && ql->next->elements[0] <= data)) {
                return selist_set_insert(&ql->next, data);
            }
            else {
                ql->elements[ql->num_elements++] = data;
            }
            return 1;
        }
        else {
            int i;
            /* TODO: OPT | binary search */
            for (i = 0; i != ql->num_elements; ++i) {
                if (data < ql->elements[i]) {
                    selist_insert(qlp, i, data);
                    return 1;
                }
                if (data == ql->elements[i]) {
                    return 0;
                }
            }
        }
    }
    selist_push(qlp, data);
    return 1;
}

int selist_set_insert_ex(struct selist **qlp, void *data, int(*cmp_cb)(const void *lhs, const void *rhs))
{
    if (*qlp) {
        selist *ql = *qlp;
        if (ql->num_elements > 0 && cmp_cb(ql->elements[ql->num_elements - 1], data) < 0) {
            if (ql->num_elements == LIST_MAXSIZE || (ql->next
                && cmp_cb(ql->next->elements[0], data) <= 0)) {
                return selist_set_insert_ex(&ql->next, data, cmp_cb);
            }
            else {
                ql->elements[ql->num_elements++] = data;
            }
            return 1;
        }
        else {
            int i;
            /* TODO: OPT | binary search */
            for (i = 0; i != ql->num_elements; ++i) {
                int cmpi = cmp_cb(data, ql->elements[i]);
                if (cmpi < 0) {
                    selist_insert(qlp, i, data);
                    return 1;
                }
                if (cmpi == 0) {
                    return 0;
                }
            }
        }
    }
    selist_push(qlp, data);
    return 1;
}

int selist_set_find(struct selist **qlp, int *qip, const void *data)
{
    selist *ql = *qlp;
    int qi;

    while (ql && ql->elements[ql->num_elements - 1] < data) {
        ql = ql->next;
    }

    if (!ql)
        return 0;

    /* TODO: OPT | binary search */
    for (qi = 0; qi != ql->num_elements; ++qi) {
        if (ql->elements[qi] > data) {
            return 0;
        }
        if (ql->elements[qi] == data) {
            if (qip) {
                *qip = qi;
                *qlp = ql;
            }
            return 1;
        }
    }
    return 0;
}

int selist_set_find_ex(struct selist **qlp, int *qip, const void *data, int(*cmp_cb)(const void *lhs, const void *rhs))
{
    selist *ql = *qlp;
    int qi;

    while (ql && cmp_cb(ql->elements[ql->num_elements - 1], data) < 0) {
        ql = ql->next;
    }

    if (!ql)
        return 0;

    /* TODO: OPT | binary search */
    for (qi = 0; qi != ql->num_elements; ++qi) {
        int cmpi = cmp_cb(ql->elements[qi], data);
        if (cmpi > 0) {
            return 0;
        }
        if (cmpi == 0) {
            if (qip) {
                *qip = qi;
                *qlp = ql;
            }
            return 1;
        }
    }
    return 0;
}

struct selist_iter qli_init(struct selist **qlp) {
    selist_iter iter = { 0, 0, 0 };
    iter.l = *qlp;
    iter.lp = qlp;
    return iter;
}

int qli_more(selist_iter iter) {
    selist * ql = iter.l;
    int qi = iter.i;
    if (ql) {
        if (qi >= ql->num_elements) {
            iter.l = ql->next;
            iter.i = qi - ql->num_elements;
            return qli_more(iter);
        }
        return qi < ql->num_elements;
    }
    return 0;
}

void * qli_next(struct selist_iter *iter) {
    void * result = selist_get(iter->l, iter->i);
    selist_advance(&iter->l, &iter->i, 1);
    return result;
}

void * qli_get(struct selist_iter iter) {
    return selist_get(iter.l, iter.i);
}

void qli_delete(struct selist_iter *iter) {
    selist * ql = iter->l;
    selist_delete(&iter->l, iter->i);
    if (iter->l != ql && *(iter->lp) == ql) {
        *(iter->lp) = iter->l;
    }
}
#endif