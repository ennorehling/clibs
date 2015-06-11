#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CuTest.h>
#include "critbit.h"

void add_suite_critbit(CuSuite *suite);

static void test_empty(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  const char * str;

  str = (const char *)cb_find_str(&cb, "herpderp");
  CuAssertStrEquals(tc, 0, str);
  cb_clear(&cb);
  CuAssertPtrEquals(tc, 0, cb.root);
}

static void test_insert(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;
  const char * str;

  result = cb_insert_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  CuAssertPtrNotNull(tc, cb.root);

  result = cb_insert_str(&cb, "herpderp");
  CuAssertIntEquals(tc, 0, result);

  str = cb_find_str(&cb, "herpderp");
  CuAssertStrEquals(tc, "herpderp", str);
  cb_clear(&cb);
}

static void test_insert_more(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;
  const char * str;

  result = cb_insert_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "murp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  str = cb_find_str(&cb, "herp");
  CuAssertStrEquals(tc, "herp", str);
  str = cb_find_str(&cb, "derp");
  CuAssertStrEquals(tc, "derp", str);
  str = cb_find_str(&cb, "murp");
  CuAssertStrEquals(tc, "murp", str);

  cb_clear(&cb);
}

static void test_insert_reverse(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;
  const char * str;

  result = cb_insert_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "hurp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  str = cb_find_str(&cb, "herp");
  CuAssertStrEquals(tc, "herp", str);
  str = cb_find_str(&cb, "derp");
  CuAssertStrEquals(tc, "derp", str);
  str = cb_find_str(&cb, "hurp");
  CuAssertStrEquals(tc, "hurp", str);

  cb_clear(&cb);
}

static void test_erase(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;
  const char * str;

  result = cb_erase_str(&cb, "herpderp");
  CuAssertIntEquals(tc, 0, result);

  result = cb_insert_str(&cb, "herp");
  result = cb_insert_str(&cb, "derp");

  result = cb_erase_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  str = cb_find_str(&cb, "herp");
  CuAssertStrEquals(tc, 0, str);
  str = cb_find_str(&cb, "derp");
  CuAssertStrEquals(tc, "derp", str);

  result = cb_erase_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  CuAssertPtrEquals(tc, 0, cb.root);

  cb_clear(&cb);
}

static void test_find_prefix(CuTest * tc)
{
  const char * matches[4] = { 0, 0, 0, 0 };
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_find_prefix_str(&cb, "herpderp", (const void**)matches, 4, 0);
  CuAssertIntEquals(tc, 0, result);
  CuAssertStrEquals(tc, 0, matches[0]);

  result = cb_insert_str(&cb, "herp");

  /* try finding a string that is longer than any in the tree */
  result = cb_find_prefix_str(&cb, "herpderp", NULL, 0, 0);
  CuAssertIntEquals(tc, 0, result);

  result = cb_find_prefix_str(&cb, "", (const void**)matches, 4, 0);
  CuAssertIntEquals(tc, 1, result);
  CuAssertStrEquals(tc, "herp", matches[0]);

  result = cb_find_prefix_str(&cb, "", (const void**)matches, 4, 1);
  CuAssertIntEquals(tc, 0, result);

  result = cb_insert_str(&cb, "herpes");

  result = cb_find_prefix_str(&cb, "", (const void**)matches, 4, 0);
  CuAssertIntEquals(tc, 2, result);
  
  result = cb_insert_str(&cb, "herpderp");
  result = cb_insert_str(&cb, "derp");

  result = cb_find_prefix_str(&cb, "", (const void**)matches, 4, 2);
  CuAssertIntEquals(tc, 2, result);

  /* silly edge-case, we expect no results, even if there are matches */
  result = cb_find_prefix_str(&cb, "herp", NULL, 0, 0);
  CuAssertIntEquals(tc, 0, result);

  result = cb_find_prefix_str(&cb, "herp", (const void**)matches, 4, 0);
  CuAssertIntEquals(tc, 3, result);
  CuAssertStrEquals(tc, "herp", matches[0]);
  CuAssertStrEquals(tc, "herpderp", matches[1]);
  CuAssertStrEquals(tc, "herpes", matches[2]);
  CuAssertStrEquals(tc, 0, matches[3]);

  matches[0] = 0;
  result = cb_find_prefix_str(&cb, "herp", (const void**)matches, 4, 3);
  CuAssertIntEquals(tc, 0, result);
  CuAssertStrEquals(tc, 0, matches[0]);
}

static int count_cb(const void * match, const void * key, size_t keylen, void * cbdata)
{
  int * result = (int *)cbdata;
  ++*result;
  return memcmp(match, key, keylen);
}

static int ordered_cb(const void * match, const void * key, size_t keylen, void * cbdata)
{
  const void ** prevptr = (const void **)cbdata;
  const void * prev = *prevptr;

  *prevptr = match;
  if (!key) return -1;
  if (memcmp(match, key, keylen)!=0) return -2;
  if (prev) {
    if (strcmp((const char *)match, (const char *)prev)<0) {
      return -3;
    }
  }
  return 0;
}

static void test_foreach(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result, counter;
  const char * prev;

  counter = 0;
  result = cb_foreach(&cb, "herpderp", strlen("herpderp"), count_cb, &counter);
  CuAssertIntEquals(tc, 0, result);
  CuAssertIntEquals(tc, 0, counter);

  result = cb_insert_str(&cb, "herp");

  /* try finding a string that is longer than any in the tree */
  counter = 0;
  result = cb_foreach(&cb, "herpderp", strlen("herpderp"), count_cb, &counter);
  CuAssertIntEquals(tc, 0, result);
  CuAssertIntEquals(tc, 0, counter);

  counter = 0;
  result = cb_foreach(&cb, "", 0, count_cb, &counter);
  CuAssertIntEquals(tc, 0, result);
  CuAssertIntEquals(tc, 1, counter);

  result = cb_insert_str(&cb, "derp");
  result = cb_insert_str(&cb, "herpes");
  result = cb_insert_str(&cb, "herpderp");

  counter = 0;
  result = cb_foreach(&cb, "", 0, count_cb, &counter);
  CuAssertIntEquals(tc, 0, result);
  CuAssertIntEquals(tc, 4, counter);

  counter = 0;
  result = cb_foreach(&cb, "herp", 0, count_cb, &counter);
  CuAssertIntEquals(tc, 0, result);
  CuAssertIntEquals(tc, 4, counter);

  /* keys should be processed in order */
  prev = 0;
  result = cb_foreach(&cb, "", 0, ordered_cb, (void *)&prev);
  CuAssertIntEquals(tc, 0, result);
}

static void test_insert_duplicates(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_insert_str(&cb, "herpderp");
  result = cb_insert_str(&cb, "herpderp");
  CuAssertIntEquals(tc, 0, result);
  result = cb_erase_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  CuAssertPtrEquals(tc, 0, cb.root);

  cb_clear(&cb);
}

static void test_keyvalue(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result, i = 0x0F0E0D0C;
  char buffer[10];
  const void * matches[2];
  const char * key = "herp";
  size_t len = strlen(key);

  len = cb_new_kv(key, len, &i, sizeof(int), buffer);
  CuAssertIntEquals(tc, (int)(strlen(buffer)+1+sizeof(int)), (int)len);
  CuAssertStrEquals(tc, buffer, key);
  cb_get_kv(buffer, &result, sizeof(int));
  CuAssertIntEquals(tc, i, result);

  result = cb_insert(&cb, buffer, len);
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_find_prefix(&cb, key, strlen(key)+1, matches, 2, 0);
  CuAssertIntEquals(tc, 1, result);
  cb_get_kv(matches[0], &result, sizeof(int));
  CuAssertIntEquals(tc, i, result);
}

static void test_clear(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();

  CuAssertIntEquals(tc, 1, cb_insert_str(&cb, "herp"));
  CuAssertPtrNotNull(tc, cb_find_str(&cb, "herp"));
  cb_clear(&cb);
  CuAssertStrEquals(tc, 0, cb_find_str(&cb, "herp"));
  CuAssertIntEquals(tc, 1, cb_insert_str(&cb, "herp"));
  CuAssertPtrNotNull(tc, cb_find_str(&cb, "herp"));
}

void add_suite_critbit(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_empty);
  SUITE_ADD_TEST(suite, test_insert);
  SUITE_ADD_TEST(suite, test_insert_more);
  SUITE_ADD_TEST(suite, test_insert_reverse);
  SUITE_ADD_TEST(suite, test_erase);
  SUITE_ADD_TEST(suite, test_foreach);
  SUITE_ADD_TEST(suite, test_find_prefix);
  SUITE_ADD_TEST(suite, test_insert_duplicates);
  SUITE_ADD_TEST(suite, test_keyvalue);
  SUITE_ADD_TEST(suite, test_clear);
}
