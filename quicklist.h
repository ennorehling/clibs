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

#ifndef _QUICKLIST_H
#define _QUICKLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* see http://en.wikipedia.org/wiki/Unrolled_linked_list */
  typedef struct quicklist quicklist;
  void *ql_get(const struct quicklist *ql, int index);
  int ql_length(const struct quicklist *ql);
  int ql_empty(const struct quicklist *ql);
  struct quicklist * ql_push(struct quicklist **qlp, void *data);
  int ql_delete(struct quicklist **qlp, int index);
  int ql_insert(struct quicklist **qlp, int index, void *data);
  void ql_foreach(struct quicklist *ql, void (*cb) (void *));
  int ql_advance(struct quicklist **iterator, int *index, int stride);
  void ql_free(struct quicklist *ql);
  void *ql_replace(struct quicklist *ql, int index, void *data);

/* you can use it as a set (sorted pointers)*/
  int ql_set_insert(struct quicklist **qlp, void *data);
  int ql_set_find(struct quicklist **qlp, int *qip, const void *data);
  int ql_set_remove(struct quicklist **qlp, void *data);

#ifdef __cplusplus
}
#endif
#endif
