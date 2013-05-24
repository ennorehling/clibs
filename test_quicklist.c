#include <stdio.h>
#include <string.h>
#include <CuTest.h>
#include "quicklist.h"

static const char * data = "Lorem ipsum dolor sit amet, "
  "consectetur adipisicing elit, sed do eiusmod tempor "
  "incididunt ut labore et dolore magna aliqua.";

static void test_insert(CuTest * tc)
{
  struct quicklist *ql = NULL;
  int i;
  for (i = 0; i != 32; ++i) {
    CuAssertIntEquals(tc, i, ql_length(ql));
    ql_insert(&ql, 0, (void *)(data+31-i));
  }
  for (i = 0; i != 32; ++i) {
    CuAssertPtrEquals(tc, (void *)(data+i), ql_get(ql, i));
  }
}

static void test_empty_list(CuTest * tc)
{
  CuAssertPtrEquals(tc, 0, ql_replace(0, 0, (void *)data));
  CuAssertPtrEquals(tc, 0, ql_get(0, 0));
  CuAssertPtrEquals(tc, 0, ql_get(0, 1));
  CuAssertIntEquals(tc, 0, ql_length(0));
}

static void test_insert_delete_gives_null(CuTest * tc)
{
  struct quicklist *ql = NULL;
  CuAssertIntEquals(tc, 1, ql_empty(ql));
  ql_push(&ql, (void *)data);
  CuAssertIntEquals(tc, 0, ql_empty(ql));
  ql_delete(&ql, 0);
  CuAssertPtrEquals(tc, 0, ql);
}

static void test_replace(CuTest * tc)
{
  struct quicklist *ql = NULL;
  void * a;
  ql_push(&ql, (void *)data);
  a = ql_replace(ql, 0, (void *)(data+1));
  CuAssertPtrEquals(tc, (void *)data, a);
  CuAssertPtrEquals(tc, (void *)(data+1), ql_get(ql, 0));
  CuAssertPtrEquals(tc, 0, ql_replace(0, 0, (void *)data));
}

static void test_set_insert(CuTest * tc)
{
  struct quicklist *ql = NULL;
  int a, qi;
  a = ql_set_insert(&ql, (void *)data);
  CuAssertIntEquals(tc, 1, ql_length(ql));
  CuAssertIntEquals(tc, 0, a);
  a = ql_set_insert(&ql, (void *)(data+1));
  CuAssertIntEquals(tc, 2, ql_length(ql));
  CuAssertIntEquals(tc, 0, a);
  a = ql_set_insert(&ql, (void *)data);
  CuAssertIntEquals(tc, 2, ql_length(ql));
  CuAssertIntEquals(tc, 1, a);
  a = ql_set_insert(&ql, (void *)(data+2));
  CuAssertIntEquals(tc, 0, a);
  CuAssertIntEquals(tc, 3, ql_length(ql));
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, 0));
  CuAssertPtrEquals(tc, (void *)(data+1), ql_get(ql, 1));
  CuAssertPtrEquals(tc, (void *)(data+2), ql_get(ql, 2));

  a = ql_set_find(&ql, &qi, (void *)(data+3));
  CuAssertIntEquals(tc, 0, a);
  a = ql_set_find(&ql, &qi, (void *)data);
  CuAssertIntEquals(tc, 1, a);
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, qi));
}

static void test_set_remove(CuTest * tc)
{
  struct quicklist *ql = NULL, *q2;
  int a;

  ql_set_insert(&ql, (void *)data);
  ql_set_insert(&ql, (void *)(data+1));
  ql_set_insert(&ql, (void *)(data+2));

  q2 = ql;

  a = ql_set_remove(&ql, (void *)(data+1));
  CuAssertPtrEquals(tc, q2, ql);
  CuAssertIntEquals(tc, 1, a);
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, 0));
  CuAssertPtrEquals(tc, (void *)(data+2), ql_get(ql, 1));
  CuAssertIntEquals(tc, 2, ql_length(ql));

  a = ql_set_remove(&ql, (void *)(data+1));
  CuAssertPtrEquals(tc, q2, ql);
  CuAssertIntEquals(tc, 0, a);

  ql_set_remove(&ql, (void *)data);
  ql_set_remove(&ql, (void *)(data+2));
  CuAssertPtrEquals(tc, 0, ql);
}

static void test_set_find(CuTest * tc)
{
  struct quicklist *ql = NULL, *q2;
  int a, qi;

  for (a = 0; a != 32; ++a) {
    ql_set_insert(&ql, (void *)(data + a));
  }

  q2 = ql;
  a = ql_set_find(&q2, 0, (void *)(data + 31));
  CuAssertIntEquals(tc, 1, a);
  CuAssertPtrEquals(tc, ql, q2);

  q2 = ql;
  a = ql_set_find(&ql, &qi, (void *)data);
  CuAssertIntEquals(tc, 1, a);
  CuAssertIntEquals(tc, 0, qi);
  CuAssertPtrEquals(tc, ql, q2);

  q2 = ql;
  a = ql_set_find(&ql, &qi, (void *)(data + 31));
  CuAssertIntEquals(tc, 1, a);
  CuAssertPtrEquals(tc, (void *)(data + 31), ql_get(ql, qi));
  CuAssertTrue(tc, ql != q2);
}

static void test_advance(CuTest * tc)
{
  struct quicklist *ql = NULL, *qli;
  int i, n = 31;
  for (i = 0; i != 32; ++i) {
    ql_insert(&ql, 0, (void *)(data + i));
  }
  for (i = 0, qli = ql; qli; ql_advance(&qli, &i, 1), n--) {
    void * g = ql_get(qli, i);
    CuAssertPtrEquals(tc, (void *)(data + n), g);
  }
}

static void test_push(CuTest * tc)
{
  struct quicklist *result, *ql = NULL;
  CuAssertIntEquals(tc, 0, ql_length(ql));
  result = ql_push(&ql, (void *)data);
  CuAssertPtrEquals(tc, result, ql);
  CuAssertIntEquals(tc, 1, ql_length(ql));
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, 0));
  ql_delete(&ql, 0);
  CuAssertPtrEquals(tc, 0, ql);
}

static void test_push_returns_end(CuTest * tc)
{
  struct quicklist *result, *ql = NULL;
  int i=0, qi=0;
  do {
    result = ql_push(&ql, 0);
    i++;
  } while (result==ql);
  ql_advance(&ql, &qi, i-1);
  CuAssertPtrEquals(tc, result, ql);
}

static void test_push_doesnt_invalidate_iterator(CuTest * tc)
{
  struct quicklist *list=NULL, *ql = NULL;
  int i, qi=0;
  ql_push(&list, (void*)data);
  ql = list;
  for (i=0;i!=42;++i) {
    void * n;
    n = ql_get(ql, qi);
    CuAssertPtrEquals(tc, (void *)(data + i), n);
    ql_push(&list, (void *)(data + (i*2+1)));
    ql_advance(&ql, &qi, 1);
    ql_push(&list, (void *)(data + (i*2+2)));
  }
}

static void test_delete_edgecases(CuTest * tc)
{
  struct quicklist *ql = NULL;
  ql_delete(&ql, 0);
  CuAssertPtrEquals(tc, 0, ql);
  ql_push(&ql, (void *)data);
  ql_delete(&ql, -1);
  ql_delete(&ql, 32);
  CuAssertIntEquals(tc, 1, ql_length(ql));
}

static void test_insert_many(CuTest * tc)
{
  struct quicklist *ql = NULL;
  int i;
  for (i = 0; i != 32; ++i) {
    ql_push(&ql, (void *)(data+i));
  }
  for (i = 0; i != 32; ++i) {
    CuAssertIntEquals(tc, 32 - i, ql_length(ql));
    CuAssertPtrEquals(tc, (void *)(data + i), ql_get(ql, 0));
    CuAssertPtrEquals(tc, (void *)(data + 31), ql_get(ql, ql_length(ql) - 1));
    ql_delete(&ql, 0);
  }
  CuAssertPtrEquals(tc, 0, ql);
}

static void test_delete_rand(CuTest * tc)
{
  struct quicklist *ql = NULL;
  int i;
  for (i = 0; i != 32; ++i) {
    ql_push(&ql, (void *)(data + i));
  }
  CuAssertIntEquals(tc, 32, ql_length(ql));
  ql_delete(&ql, 0);
  CuAssertPtrEquals(tc, (void *)(data + 1), ql_get(ql, 0));
  CuAssertIntEquals(tc, 31, ql_length(ql));
  ql_delete(&ql, 30);
  CuAssertIntEquals(tc, 30, ql_length(ql));
}

void add_suite_quicklist(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_advance);
  SUITE_ADD_TEST(suite, test_replace);
  SUITE_ADD_TEST(suite, test_push);
  SUITE_ADD_TEST(suite, test_push_returns_end);
  SUITE_ADD_TEST(suite, test_insert);
  SUITE_ADD_TEST(suite, test_set_remove);
  SUITE_ADD_TEST(suite, test_set_find);
  SUITE_ADD_TEST(suite, test_empty_list);
  SUITE_ADD_TEST(suite, test_insert_delete_gives_null);
  SUITE_ADD_TEST(suite, test_insert_many);
  SUITE_ADD_TEST(suite, test_delete_rand);
  SUITE_ADD_TEST(suite, test_delete_edgecases);
  SUITE_ADD_TEST(suite, test_set_insert);
  SUITE_ADD_TEST(suite, test_push_doesnt_invalidate_iterator);
}

int main(int argc, char ** argv)
{
  CuString *output = CuStringNew();
  CuSuite *suite = CuSuiteNew();

  add_suite_quicklist(suite);

  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
  return suite->failCount;
}

