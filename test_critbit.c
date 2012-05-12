#include <cutest/CuTest.h>
#include <stdlib.h>
#include "critbit.h"

static void test_empty(CuTest * tc)
{
  critbit_tree cb = {0};
  int result;

  result = cb_find(&cb, "herpderp");
  CuAssertIntEquals(tc, result, CB_ENOMORE);
  result = cb_find(&cb, "herpderp");
  CuAssertIntEquals(tc, result, CB_ENOMORE);
  cb_free(&cb, 0);
  CuAssertPtrEquals(tc, cb.root, 0);
}

static void test_insert(CuTest * tc)
{
  critbit_tree cb = {0};
  int result;

  result = cb_insert(&cb, "herpderp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);
  CuAssertPtrNotNull(tc, cb.root);

  result = cb_insert(&cb, "herpderp");
  CuAssertIntEquals(tc, result, CB_ENOMORE);

  result = cb_find(&cb, "herpderp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);
  cb_free(&cb, 0);
}

static void test_insert_more(CuTest * tc)
{
  critbit_tree cb = {0};
  int result;

  result = cb_insert(&cb, "herp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);
  result = cb_insert(&cb, "derp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);
  result = cb_insert(&cb, "murp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);

  result = cb_find(&cb, "herp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);
  result = cb_find(&cb, "derp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);
  result = cb_find(&cb, "murp");
  CuAssertIntEquals(tc, result, CB_SUCCESS);

  cb_free(&cb, 0);
}

static void test_erase(CuTest * tc)
{
  critbit_tree cb = {0};
  int result;

  cb_insert(&cb, "herpderp");
  result = cb_find(&cb, "herpderp");
  CuAssertIntEquals(tc, result, 1);
  /*
  cb_erase(&cb, "herpderp");
  result = cb_find(&cb, "herpderp");
  CuAssertIntEquals(tc, result, 0);
  */
  cb_free(&cb, 0);
}

void add_suite_critbit(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_empty);
  SUITE_ADD_TEST(suite, test_insert);
  SUITE_ADD_TEST(suite, test_insert_more);
  SUITE_ADD_TEST(suite, test_erase);
}
