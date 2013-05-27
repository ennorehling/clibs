/*
Copyright (c) 2010-2011, Enno Rehling <enno@eressea.de>

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

#include "quicklist.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define QL_MAXSIZE 14 /* max. number of elements unrolled into one node */
#define QL_LIMIT 7 /* this many or fewer number in a node => attempt merge */

/* The total size of this struct is 64 bytes on a 32-bit system with
 * normal alignment. YMMV, so on a 64-bit system, twiddle the
 * constants above */
struct quicklist {
  struct quicklist *next;
  int num_elements;
  void *elements[QL_MAXSIZE];
};

void *ql_get(const quicklist * ql, int i)
{
    assert(ql);
    return (i < ql->num_elements) ? ql->elements[i] : ql_get(ql->next, i - ql->num_elements);
}

void *ql_replace(quicklist * ql, int i, void *data)
{
    assert(ql);
    if (i < ql->num_elements) {
        void *orig = ql->elements[i];
        ql->elements[i] = data;
        return orig;
    } else {
        return ql_replace(ql->next, i - ql->num_elements, data);
    }
}

int ql_length(const quicklist * ql)
{
    return ql ? ql->num_elements + ql_length(ql->next) : 0;
}

ql_bool ql_empty(const quicklist * ql)
{
    return !ql;
}

quicklist * ql_push(quicklist ** qlp, void *data)
{
  quicklist *ql = 0;
  while (*qlp && ((*qlp)->next || (*qlp)->num_elements == QL_MAXSIZE)) {
    qlp = &(*qlp)->next;
  }
  if (!*qlp) {
    ql = (quicklist *) malloc(sizeof(quicklist));
    ql->num_elements = 0;
    ql->next = 0;
    *qlp = ql;
  } else {
    ql = *qlp;
  }
  ql->elements[ql->num_elements++] = data;
  return ql;
}

int ql_delete(quicklist ** qlp, int i)
{
  quicklist *ql = *qlp;
  if (i < 0)
    return EINVAL;
  if (ql && i >= ql->num_elements) {
    return ql_delete(&ql->next, i - ql->num_elements);
  } else if (ql) {
    if (i + 1 < ql->num_elements) {
      memmove(ql->elements + i, ql->elements + i + 1,
        (ql->num_elements - i - 1) * sizeof(void *));
    }
    --ql->num_elements;
    if (ql->num_elements == 0) {
      *qlp = ql->next;
      free(ql);
    } else if (ql->next && ql->num_elements <= QL_LIMIT) {
      quicklist *qn = ql->next;
      if (ql->num_elements + qn->num_elements > QL_MAXSIZE) {
        memcpy(ql->elements + ql->num_elements, qn->elements, sizeof(void *));
        --qn->num_elements;
        ++ql->num_elements;
        memmove(qn->elements, qn->elements + 1,
          qn->num_elements * sizeof(void *));
      } else {
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

int ql_insert(quicklist ** qlp, int i, void *data)
{
  quicklist *ql = *qlp;
  if (ql) {
    if (i >= QL_MAXSIZE) {
      return ql_insert(&ql->next, i - ql->num_elements, data);
    } else if (ql->num_elements < QL_MAXSIZE) {
      memmove(ql->elements + i + 1, ql->elements + i,
        (ql->num_elements - i) * sizeof(void *));
      ql->elements[i] = data;
      ++ql->num_elements;
    } else {
      quicklist *qn = (quicklist *) malloc(sizeof(quicklist));
      qn->next = ql->next;
      ql->next = qn;
      qn->num_elements = ql->num_elements-QL_LIMIT;
      ql->num_elements = QL_LIMIT;
      memcpy(qn->elements, ql->elements + ql->num_elements,
        QL_LIMIT * sizeof(void *));
      if (i <= ql->num_elements) {
        return ql_insert(qlp, i, data);
      } else {
        return ql_insert(&ql->next, i - ql->num_elements, data);
      }
    }
  } else if (i == 0) {
    ql_push(qlp, data);
  } else {
    return EINVAL;
  }
  return 0;
}

void ql_foreach(struct quicklist *ql, void (*cb) (void *))
{
  for (; ql; ql = ql->next) {
    int i;
    for (i = 0; i != ql->num_elements; ++i) {
      cb(ql->elements[i]);
    }
  }
}

void ql_foreachx(struct quicklist *ql, void (*cb) (void *, void *), void *x)
{
  for (; ql; ql = ql->next) {
    int i;
    for (i = 0; i != ql->num_elements; ++i) {
      cb(ql->elements[i], x);
    }
  }
}

int ql_advance(struct quicklist **iterator, int *index, int stride)
{
  quicklist *ql = *iterator;
  int i = *index;
  if (!ql || i < 0 || stride < 0) {
    return ERANGE;
  } else if (i + stride < ql->num_elements) {
    *index = i + stride;
    return 0;
  } else {
    *index = i - ql->num_elements + stride;
    *iterator = ql->next;
    return ql_advance(iterator, index, 0);
  }
}

void ql_free(struct quicklist *ql)
{
  while (ql) {
    quicklist * qn = ql;
    ql = ql->next;
    free(qn);
  }
}

ql_bool ql_set_remove(struct quicklist **qlp, void *data)
{
  int qi;
  quicklist *ql = *qlp;

  if (!ql)
    return ql_false;

  for (qi = 0; qi != ql->num_elements; ++qi) {
    void *qd = ql_get(ql, qi);
    if (qd == data) {
      return ql_delete(qlp, qi) == 0;
    }
  }
  return ql_set_remove(&ql->next, data);
}

ql_bool ql_set_insert(struct quicklist **qlp, void *data)
{
  if (*qlp) {
    quicklist *ql = *qlp;
    if (ql->num_elements > 0 && ql->elements[ql->num_elements - 1] < data) {
      if (ql->num_elements == QL_MAXSIZE || (ql->next
          && ql->next->elements[0] <= data)) {
        return ql_set_insert(&ql->next, data);
      } else {
        ql->elements[ql->num_elements++] = data;
      }
      return ql_true;
    } else {
      int i;
      /* TODO: OPT | binary search */
      for (i = 0; i != ql->num_elements; ++i) {
        if (data < ql->elements[i]) {
          ql_insert(qlp, i, data);
          return ql_true;
        }
        if (data == ql->elements[i]) {
          return ql_false;
        }
      }
    }
  }
  ql_push(qlp, data);
  return ql_true;
}

ql_bool ql_set_find(struct quicklist **qlp, int *qip, const void *data)
{
  quicklist *ql = *qlp;
  int qi;

  while (ql && ql->elements[ql->num_elements - 1] < data) {
    ql = ql->next;
  }

  if (!ql)
    return ql_false;

  /* TODO: OPT | binary search */
  for (qi = 0; qi != ql->num_elements; ++qi) {
    if (ql->elements[qi] > data) {
      return ql_false;
    }
    if (ql->elements[qi] == data) {
      if (qip) {
        *qip = qi;
        *qlp = ql;
      }
      return ql_true;
    }
  }
  return ql_false;
}

struct ql_iter qli_init(struct quicklist *ql) {
  ql_iter iter = { 0 };
  iter.l = ql;
  return iter;
}

ql_bool qli_more(ql_iter iter) {
    return iter.l!=0;
}

void * qli_next(struct ql_iter *iter) {
    void * result = ql_get(iter->l, iter->i);
    ql_advance(&iter->l, &iter->i, 1);
    return result;
}

void ql_map_reduce(struct quicklist *ql, void (*mapfunc)(void *entry, void *data), void(*reducefunc)(void *data, void *result), void *data, void *result)
{
  int qi;
  for (qi = 0; ql; ql_advance(&ql, &qi, 1)) {
    mapfunc(ql_get(ql, qi), data);
    reducefunc(data, result);
  }
}
