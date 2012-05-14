#include <cutest/CuTest.h>
#include <stdlib.h>
#include <string.h>
#include "critbit.h"

static void test_empty(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_find_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_ENOMORE, result);
  cb_clear(&cb);
  CuAssertPtrEquals(tc, 0, cb.root);
}

static void test_insert(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_insert_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  CuAssertPtrNotNull(tc, cb.root);

  result = cb_insert_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_ENOMORE, result);

  result = cb_find_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  cb_clear(&cb);
}

static void test_insert_more(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_insert_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "murp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  result = cb_find_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_find_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_find_str(&cb, "murp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  cb_clear(&cb);
}

static void test_insert_reverse(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_insert_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "hurp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_insert_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  result = cb_find_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_find_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_find_str(&cb, "hurp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

  cb_clear(&cb);
}

static void test_erase(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_erase_str(&cb, "herpderp");
  CuAssertIntEquals(tc, 0, result);

  result = cb_insert_str(&cb, "herp");
  result = cb_insert_str(&cb, "derp");

  result = cb_erase_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  result = cb_find_str(&cb, "herp");
  CuAssertIntEquals(tc, CB_ENOMORE, result);
  result = cb_find_str(&cb, "derp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);

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

  result = cb_find_prefix_str(&cb, "herpderp", matches, 4, 0);
  CuAssertIntEquals(tc, CB_ENOMORE, result);
  CuAssertStrEquals(tc, 0, matches[0]);

  result = cb_insert_str(&cb, "herp");

  /* try finding a string that is longer than any in the tree */
  result = cb_find_prefix_str(&cb, "herpderp", NULL, 0, 0);
  CuAssertIntEquals(tc, 0, result);

  result = cb_find_prefix_str(&cb, "", matches, 4, 0);
  CuAssertIntEquals(tc, 1, result);
  CuAssertStrEquals(tc, "herp", matches[0]);

  result = cb_find_prefix_str(&cb, "", matches, 4, 1);
  CuAssertIntEquals(tc, 0, result);

  result = cb_insert_str(&cb, "herpes");

  result = cb_find_prefix_str(&cb, "", matches, 4, 0);
  CuAssertIntEquals(tc, 2, result);
  
  result = cb_insert_str(&cb, "herpderp");
  result = cb_insert_str(&cb, "derp");

  result = cb_find_prefix_str(&cb, "", matches, 4, 2);
  CuAssertIntEquals(tc, 2, result);

  /* silly edge-case, we expect no results, even if there are matches */
  result = cb_find_prefix_str(&cb, "herp", NULL, 0, 0);
  CuAssertIntEquals(tc, 0, result);

  result = cb_find_prefix_str(&cb, "herp", matches, 4, 0);
  CuAssertIntEquals(tc, 3, result);
  CuAssertStrEquals(tc, "herp", matches[0]);
  CuAssertStrEquals(tc, "herpderp", matches[1]);
  CuAssertStrEquals(tc, "herpes", matches[2]);
  CuAssertStrEquals(tc, 0, matches[3]);

  matches[0] = 0;
  result = cb_find_prefix_str(&cb, "herp", matches, 4, 3);
  CuAssertIntEquals(tc, CB_ENOMORE, result);
  CuAssertStrEquals(tc, 0, matches[0]);
}

static void test_insert_duplicates(CuTest * tc)
{
  critbit_tree cb = CRITBIT_TREE();
  int result;

  result = cb_insert_str(&cb, "herpderp");
  result = cb_insert_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_ENOMORE, result);
  result = cb_erase_str(&cb, "herpderp");
  CuAssertIntEquals(tc, CB_SUCCESS, result);
  CuAssertPtrEquals(tc, 0, cb.root);

  cb_clear(&cb);
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
}
