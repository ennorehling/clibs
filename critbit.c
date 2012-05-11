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
  char mask;
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

struct critbit_node * internal_node(void * ptr) {
  if (ptr) {
    ptrdiff_t numvalue = (char*)ptr - (char*)0;
    if ((numvalue&1)==0) return (struct critbit_node *)ptr;
  }
  return 0;
}

void * make_external_node(const char * key)
{
  ptrdiff_t numvalue = key - (const char*)0;  
  assert((numvalue&1) == 0);
  return (void*)(numvalue+1);
}

struct critbit_node * make_internal_node(void)
{
  struct critbit_node *node = (struct critbit_node *)malloc(sizeof(struct critbit_node));
  return node;
}

static void cb_free_node(void * ptr, void (*release_cb)(void *))
{
  if (decode_pointer(&ptr)==INTERNAL_NODE) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    cb_free_node(node->child[0], release_cb);
    cb_free_node(node->child[1], release_cb);
    free(node);
  } else if (release_cb) {
    release_cb(ptr);
  }
}

void cb_free(critbit_tree * cb, void (*release_cb)(void *))
{
  assert(cb);
  if (cb->root) cb_free_node(cb->root, release_cb);
}

static void cb_insert_node(void ** iter, const char * key, size_t keylen)
{
  void * ptr = *iter;
  if (decode_pointer(&ptr)==INTERNAL_NODE) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    int branch = (keylen<=node->byte) ? 0 : ((1+((key[node->byte]|node->mask)&0xFF))>>8);
    cb_insert_node(&node->child[branch], key, keylen);
  } else {
    const char * ikey = key;
    const char * iptr = (const char *)ptr;
    int mask, branch;
    unsigned int byte = 0;
    struct critbit_node * node = make_internal_node();

    while (*ikey && *iptr && *ikey==*iptr) {
      ++ikey;
      ++iptr;
      ++byte;
    }
    node->byte = byte;
    mask = *ikey ^ *iptr; /* these are all the bits that differ */
    mask |= mask>>1;
    mask |= mask>>2;
    mask |= mask>>4; /* now, every bit up to the MSB is set to 1 */
    mask = (mask&~(mask>>1))^0xFF;
    node->mask = (char)mask;
    branch = ((1+((*ikey|node->mask)&0xFF))>>8);
    node->child[1-branch] = *iter;
    node->child[branch] = make_external_node(key);
    *iter = (void *)node;
  }
}

void cb_insert(critbit_tree * cb, const char * key)
{
  assert(cb);
  assert(key);
  if (!cb->root) {
    cb->root = make_external_node(key);
  } else {
    cb_insert_node(&cb->root, key, strlen(key));
  }
}

static int cb_find_node(void * ptr, const char * key, size_t keylen)
{
  if (decode_pointer(&ptr)==INTERNAL_NODE) {
    struct critbit_node * node = (struct critbit_node *)ptr;
    int branch = (keylen<=node->byte) ? 0 : ((1+((key[node->byte]|node->mask)&0xFF))>>8);
    return cb_find_node(node->child[branch], key, keylen);
  }
  return strcmp(key, (const char*)ptr)==0;
}

int cb_find(critbit_tree * cb, const char * key)
{
  assert(cb);
  assert(key);
  if (!cb->root) return 0;
  return cb_find_node(cb->root, key, strlen(key));
}
