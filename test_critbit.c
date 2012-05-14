#include <cutest/CuTest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "critbit.h"

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
  int result, i;

  for (i=0;i!=10;++i) {
    size_t len = sizeof(int);
    char key[20];
    void * kv;

    sprintf(key, "%d", i);
    kv = cb_alloc_kv(key, &i, &len);
    CuAssertIntEquals(tc, strlen(key)+1+sizeof(int), len);
    result = cb_insert(&cb, kv, len);
    cb_free_kv(kv);
    CuAssertIntEquals(tc, CB_SUCCESS, result);
  }

  for (i=0;i!=10;++i) {
    char key[20];
    int value, result;
    const void * match[2];

    sprintf(key, "%d", i);
    result = cb_find_prefix(&cb, key, strlen(key)+1, match, 2, 0);
    CuAssertIntEquals(tc, 1, result);
    CuAssertStrEquals(tc, key, (const char *)match[0]);

    cb_get_kv(match[0], &value, sizeof(int));
    CuAssertIntEquals(tc, i, value);
  }
}

void add_suite_critbit(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_empty);
  SUITE_ADD_TEST(suite, test_insert);
  SUITE_ADD_TEST(suite, test_insert_more);
  SUITE_ADD_TEST(suite, test_insert_reverse);
  SUITE_ADD_TEST(suite, test_erase);
  SUITE_ADD_TEST(suite, test_find_prefix);
  SUITE_ADD_TEST(suite, test_insert_duplicates);
  SUITE_ADD_TEST(suite, test_keyvalue);
}
