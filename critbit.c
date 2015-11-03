/*
Copyright (c) 2012, Enno Rehling <enno@eressea.de>

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

#include "critbit.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* see http://cr.yp.to/critbit.html */
struct critbit_node {
  void * child[2];
  size_t byte;
  unsigned int mask;
};

#define EXTERNAL_NODE 0
#define INTERNAL_NODE 1

static int decode_pointer(void ** ptr)
{
  ptrdiff_t numvalue = (char*)*ptr - (char*)0;
  if (numvalue&1) {
    *ptr = (void*)(numvalue-1);
    return EXTERNAL_NODE;
  }
  return INTERNAL_NODE;
}

static void * make_external_node(const void * key, size_t keylen)
{
  char * data = (char *)malloc(sizeof(size_t) + keylen);
#ifndef NDEBUG
  ptrdiff_t numvalue = (char *)data - (char*)0;
  assert((numvalue&1)==0);
#endif
  assert(keylen);
  memcpy(data, &keylen, sizeof(size_t));
  memcpy(data+sizeof(size_t), key, keylen);
  return (void*)(data+1);
}

static void from_external_node(void * ptr, void **key, size_t *keylen)
{
  unsigned char * bytes = (unsigned char *)ptr;
#ifndef NDEBUG
  ptrdiff_t numvalue = bytes - (unsigned char*)0;
  assert(numvalue && (numvalue&1)==0);
  assert(key && keylen);
#endif
  memcpy(keylen, bytes, sizeof(size_t));
  *key = bytes+sizeof(size_t);
}

static struct critbit_node * make_internal_node(void)
{
  struct critbit_node *node = (struct critbit_node *)malloc(sizeof(struct critbit_node));
  return node;
}

static void cb_free_node(void * ptr)
{
  if (decode_pointer(&ptr)==INTERNAL_NODE) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    cb_free_node(node->child[0]);
    cb_free_node(node->child[1]);
    free(node);
  } else {
    free(ptr);
  }
}

void cb_clear(critbit_tree * cb)
{
  if (cb->root) {
    cb_free_node(cb->root);
    cb->root = 0;
  }
}

static int cb_less(const struct critbit_node * a, const struct critbit_node * b)
{
  return a->byte<b->byte || (a->byte==b->byte && a->mask < b->mask);
}

int cb_insert(critbit_tree * cb, const void * key, size_t keylen)
{
  assert(cb);
  assert(key);
  if (!cb->root) {
    cb->root = make_external_node(key, keylen);
    return 1;
  } else {
    void ** iter = &cb->root;
    struct critbit_node * prev = 0;
    for (;;) {
      void * ptr = *iter;
      if (decode_pointer(&ptr)==INTERNAL_NODE) {
        struct critbit_node * node = (struct critbit_node *)ptr;
        unsigned char * bytes = (unsigned char *)key;
        int branch = (keylen<=node->byte) ? 0 : ((1+((bytes[node->byte]|node->mask)&0xFF))>>8);
        iter = &node->child[branch];
        prev = node;
      } else {
        unsigned char *iptr, *bytes = (unsigned char *)key, *ikey = bytes;
        void * vptr;
        unsigned int mask, byte = 0;
        int branch;
        size_t len;
        struct critbit_node * node;

        from_external_node(ptr, &vptr, &len);

        for (iptr=vptr;byte<keylen && byte<len && *ikey==*iptr;) {
          ++ikey;
          ++iptr;
          ++byte;
        }

        if (byte==keylen && byte==len) {
          return 0; /* duplicate entry */
        }
        node = make_internal_node();
        node->byte = byte;
        mask = *ikey ^ *iptr; /* these are all the bits that differ */
        mask |= mask>>1;
        mask |= mask>>2;
        mask |= mask>>4; /* now, every bit up to the MSB is set to 1 */
        mask = (mask&~(mask>>1))^0xFF;
        node->mask = (unsigned char)mask;

        /* find the right place to insert, iff prev's crit-bit is later in the string than new crit-bit */
        if (prev && cb_less(node, prev)) {
          for (iter = &cb->root;;) {
            ptr = *iter;
            if (decode_pointer(&ptr)==INTERNAL_NODE) {
              struct critbit_node * next = (struct critbit_node *)ptr;
              if (cb_less(next, node)) {
                branch = ((1+((bytes[next->byte]|next->mask)&0xFF))>>8);
                iter = &next->child[branch];
              } else {
                break;
              }
            } else {
              assert(!"should never get here");
            }
          }
        }

        branch = ((1+((*ikey|node->mask)&0xFF))>>8);
        node->child[branch] = make_external_node(key, keylen);
        node->child[1-branch] = *iter;
        *iter = (void *)node;

        return CB_SUCCESS;
      }
    }
  }
}

static void * cb_find_top_i(critbit_tree * cb, const void * key, size_t keylen)
{
  void *ptr, *top = 0;
  assert(key);

  if (!cb->root) {
    return 0;
  }
  for (ptr=cb->root, top=cb->root;;) {
    void * last = ptr;
    if (decode_pointer(&ptr)==INTERNAL_NODE) {
      struct critbit_node * node = (struct critbit_node *)ptr;
      int branch;
      if (keylen<=node->byte) {
        break;
      } else {
        unsigned char * bytes = (unsigned char *)key;
        top = last;
        branch = (1+((bytes[node->byte]|node->mask)&0xFF))>>8;
        ptr = node->child[branch];
      }
    } else {
      /* we reached an external node before exhausting the key length */
      top = last;
      break;
    }
  }
  return top;
}

static int cb_find_prefix_i(void * ptr, const void * key, size_t keylen, void ** results, int numresults, int * offset, int next)
{
  assert(next<=numresults);
  if (next==numresults) {
    return next;
  } else if (decode_pointer(&ptr)==INTERNAL_NODE) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    next = cb_find_prefix_i(node->child[0], key, keylen, results, numresults, offset, next);
    if (next<numresults) {
      next = cb_find_prefix_i(node->child[1], key, keylen, results, numresults, offset, next);
    }
  } else {
    /* reached an external node */
    char * str;
    void * vptr;
    size_t len;

    from_external_node(ptr, &vptr, &len);
    str = vptr;
    if (len>=keylen && memcmp(key, str, keylen)==0) {
      if (*offset>0) {
        --*offset;
      } else {
        results[next++] = str;
      }
    }
  }
  return next;
}

int cb_find_prefix(critbit_tree * cb, const void * key, size_t keylen, void ** results, int numresults, int offset)
{
  if (numresults>0) {
    void *top = cb_find_top_i(cb, key, keylen);
    if (top) {
      /* recursively add all children except the ones from [0-offset) of top to the results */
      return cb_find_prefix_i(top, key, keylen, results, numresults, &offset, 0);
    }
  }
  return 0;
}

static int cb_foreach_i(void * ptr, const void * key, size_t keylen, int (*match_cb)(const void * match, const void * key, size_t keylen, void *), void *data)
{
  int result = 0;

  if (decode_pointer(&ptr)==INTERNAL_NODE) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    result = cb_foreach_i(node->child[0], key, keylen, match_cb, data);
    if (!result) {
      result = cb_foreach_i(node->child[1], key, keylen, match_cb, data);
    }
  } else {
    /* reached an external node */
    void * match;
    size_t len;

    from_external_node(ptr, &match, &len);
    if (len>=keylen && memcmp(key, match, keylen)==0) {
      return match_cb(match, key, keylen, data);
    }
  }
  return result;
}

int cb_foreach(critbit_tree * cb, const void * key, size_t keylen, int (*match_cb)(const void * match, const void * key, size_t keylen, void *), void *data)
{
  void *top = cb_find_top_i(cb, key, keylen);
  if (top) {
    /* recursively add all children except the ones from [0-offset) of top to the results */
    return cb_foreach_i(top, key, keylen, match_cb, data);
  }
  return 0;
}

const void * cb_find(critbit_tree * cb, const void * key, size_t keylen)
{
  void * str;
  size_t len;
  unsigned char * bytes = (unsigned char *)key;
  void * ptr;

  assert(cb);
  assert(key);
  if (!cb->root) return 0;
  for (ptr=cb->root;decode_pointer(&ptr)==INTERNAL_NODE;) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    int branch = (keylen<=node->byte) ? 0 : ((1+((bytes[node->byte]|node->mask)&0xFF))>>8);
    ptr = node->child[branch];
  }
  from_external_node(ptr, &str, &len);
  if (len>=keylen && memcmp(key, str, keylen)==0) {
    return str;
  }
  return 0;
}

int cb_erase(critbit_tree * cb, const void * key, size_t keylen)
{
  void **iter = &cb->root;
  void *ptr = *iter;
  unsigned char * bytes = (unsigned char *)key;

  if (!cb->root) return 0;

  if (decode_pointer(&ptr)==EXTERNAL_NODE) {
    free(ptr);
    cb->root = 0;
    return CB_SUCCESS;
  }

  for (;;) {
    struct critbit_node *parent = (struct critbit_node *)ptr;
    int branch, type;

    branch = (keylen<=parent->byte) ? 0 : ((1+((bytes[parent->byte]|parent->mask)&0xFF))>>8);

    ptr = parent->child[branch];
    type = decode_pointer(&ptr);
    if (type==INTERNAL_NODE) {
      iter = &parent->child[branch];
    } else {
      void * str;
      size_t len;
      from_external_node(ptr, &str, &len);
      if (len==keylen && memcmp(key, str, len)==0) {
        free(ptr);
        *iter = parent->child[1-branch];
        return CB_SUCCESS;
      }
      return 0;
    }
  }
}

size_t cb_new_kv(const char *key, size_t keylen, void * value, size_t len, void * out)
{
  char * dst = (char*)out;
  if (dst!=key) {
    memmove(dst, key, keylen);
  }
  dst[keylen] = 0;
  memmove(dst+keylen+1, value, len);
  return len+keylen+1;
}

void cb_get_kv(const void *kv, void * value, size_t len)
{
  const char * key = (const char *)kv;
  size_t keylen = strlen(key)+1;
  memmove(value, key+keylen, len);
}

void cb_get_kv_ex(void *kv, void ** value)
{
    char * key = (char *)kv;
    size_t keylen = strlen(key) + 1;
    *value = key + keylen;
}
