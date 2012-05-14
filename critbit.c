/*
Copyright (c) 2011, Enno Rehling <enno@eressea.de>

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
  unsigned char mask;
};

#define EXTERNAL_NODE 0
#define INTERNAL_NODE 1

void cb_init(critbit_tree * cb)
{
  cb->root = 0;
}

static int decode_pointer(void ** ptr)
{
  ptrdiff_t numvalue = (const char*)*ptr - (const char*)0;
  if (numvalue&1) {
    *ptr = (void*)(numvalue-1);
    return EXTERNAL_NODE;
  }
  return INTERNAL_NODE;
}

void * make_external_node(const char * key, size_t keylen)
{
  char * data = strcpy((char *)malloc(keylen + 1), key);
  ptrdiff_t numvalue = data - (const char*)0;
  assert((numvalue&1)==0);
  return (void*)(data+1);
}

struct critbit_node * make_internal_node(void)
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
  }
}

static int cb_less(const struct critbit_node * a, const struct critbit_node * b)
{
  return a->byte<b->byte || (a->byte==b->byte && a->mask < b->mask);
}

int cb_insert(critbit_tree * cb, const char * key)
{
  size_t keylen = strlen(key);
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
        int branch = (keylen<=node->byte) ? 0 : ((1+((key[node->byte]|node->mask)&0xFF))>>8);
        iter = &node->child[branch];
        prev = node;
      } else {
        const char * ikey = key;
        const char * iptr = (const char *)ptr;
        unsigned int mask, branch;
        unsigned int byte = 0;
        struct critbit_node * node = make_internal_node();

        while (*ikey && *iptr && *ikey==*iptr) {
          ++ikey;
          ++iptr;
          ++byte;
        }

        if (*ikey == *iptr) {
          return CB_ENOMORE; /* duplicate entry */
        }
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
                branch = ((1+((key[next->byte]|next->mask)&0xFF))>>8);
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

static int cb_find_prefix_i(void * ptr, const char * key, size_t keylen, const char ** results, int numresults, int * offset, int next)
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
    const char * str = (const char *)ptr;
    size_t len = strlen(str);
    if (len>=keylen && strncmp(key, str, keylen)==0) {
      if (*offset>0) {
        --*offset;
      } else {
        results[next++] = str;
      }
    }
  }
  return next;
}

int cb_find_prefix(critbit_tree * cb, const char * key, const char ** results, int numresults, int offset)
{
  void *ptr, *top = 0;
  size_t keylen;

  if (!cb->root || !numresults) {
    return 0;
  }
  keylen = strlen(key);
  for (ptr=cb->root, top=cb->root;;) {
    void * last = ptr;
    if (decode_pointer(&ptr)==INTERNAL_NODE) {
      struct critbit_node * node = (struct critbit_node *)ptr;
      int branch;
      if (keylen<=node->byte) {
        break;
      }
      top = last;
      branch = (1+((key[node->byte]|node->mask)&0xFF))>>8;
      ptr = node->child[branch];
    } else {
      /* we reached an external node before exhausting the key length */
      top = last;
      break;
    }
  }
  if (top) {
    /* recursively add all children except the ones from [0-offset) of top to the results */
    return cb_find_prefix_i(top, key, keylen, results, numresults, &offset, 0);
  }
  return 0;
}

int cb_find(critbit_tree * cb, const char * key)
{
  void * ptr;
  size_t keylen;

  assert(cb);
  assert(key);
  if (!cb->root) return CB_ENOMORE;
  for (ptr=cb->root, keylen = strlen(key);decode_pointer(&ptr)==INTERNAL_NODE;) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    int branch = (keylen<=node->byte) ? 0 : ((1+((key[node->byte]|node->mask)&0xFF))>>8);
    ptr = node->child[branch];
  }
  return strcmp(key, (const char*)ptr)==0;
}

int cb_erase(critbit_tree * cb, const char * key)
{
  size_t keylen = strlen(key);
  void **iter = &cb->root;
  void *ptr = *iter;
  if (!cb->root) return 0;

  if (decode_pointer(&ptr)==EXTERNAL_NODE) {
   
    free(ptr);
    cb->root = 0;
    return CB_SUCCESS;
  }

  for (;;) {
    struct critbit_node *parent = (struct critbit_node *)ptr;
    int branch, type;

    branch = (keylen<=parent->byte) ? 0 : ((1+((key[parent->byte]|parent->mask)&0xFF))>>8);

    ptr = parent->child[branch];
    type = decode_pointer(&ptr);
    if (type==INTERNAL_NODE) {
      iter = &parent->child[branch];
    } else {
      if (strcmp(key, (const char *)ptr)==0) {
        free(ptr);
        *iter = parent->child[1-branch];
        return CB_SUCCESS;
      }
      return 0;
    }
  }
}
