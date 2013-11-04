#include <stdio.h>
#include <string.h>
#include <CuTest.h>
#include "quicklist.h"

static const char * data = "Lorem ipsum dolor sit amet, "
  "consectetur adipisicing elit, sed do eiusmod tempor "
  "incididunt ut labore et dolore magna aliqua.";

static void test_iter_delete_all(CuTest * tc)
{
    quicklist * ql = 0;
    ql_iter iter;

    ql_push(&ql, (void *)data);
    iter = qli_init(&ql);
    qli_delete(&iter);
    CuAssertPtrEquals(tc, 0, ql);
}

static void test_loop(CuTest * tc)
{
    ql_iter iter;
    quicklist * ql = 0;
    int result = 0;

    ql_push(&ql, (void *)data);
    ql_push(&ql, (void *)(data+1));
    for (iter = qli_init(&ql);qli_more(iter);) {
        const char * c = (const char *)qli_next(&iter);
        result += *c;
    }
    CuAssertIntEquals(tc, data[0]+data[1], result);
}

static void test_more(CuTest * tc)
{
    ql_iter iter;
    quicklist * ql = 0;

    ql_push(&ql, (void *)data);
    iter = qli_init(&ql);
    CuAssertTrue(tc, qli_more(iter));
    ql_delete(&iter.l, iter.i);
    CuAssertTrue(tc, !qli_more(iter));
}

static void my_map(void *entry, void *data) {
  const char * str = (const char *)entry;
  *(int *)data = *str;
}

static void my_reduce(void *data, void *result) {
  int * d = (int *)data;
  int * r = (int *)result;
  *r += *d;
}

static void test_mapreduce(CuTest * tc)
{
  int buffer;
  int result = 0;
  struct quicklist *ql = 0;
  ql_push(&ql, (void *)data);
  ql_push(&ql, (void *)data);
  ql_map_reduce(ql, my_map, my_reduce, &buffer, &result);
  CuAssertIntEquals(tc, 152, result);
  ql_free(ql);
}

static void my_callbackx(void *entry, void *x) {
    const char * str = (const char *)entry;
    *(int *)x += str[0];
}

static int gtotal = 0;
static void my_callback(void *entry) {
    const char * str = (const char *)entry;
    gtotal += str[0];
}

static void test_foreach(CuTest *tc) {
    int total = 0;
    struct quicklist *ql = 0;
    ql_push(&ql, (void *)data);
    ql_push(&ql, (void *)data);
    ql_foreach(ql, my_callback);
    CuAssertIntEquals(tc, 152, gtotal);
    ql_foreachx(ql, my_callbackx, &total);
    CuAssertIntEquals(tc, 152, total);
    ql_free(ql);
}

static ql_bool cb_match_int(const void*a, const void*b) {
    return *(const int *)a == *(const int *)b;
}

static void test_find_cb(CuTest *tc) {
    int a = 42;
    int b = 23;
    int c = 42;
    struct quicklist *ql = 0;
    struct quicklist *il;
    int i;

    ql_push(&ql, (void *)&a);
    ql_push(&ql, (void *)&b);
    ql_push(&ql, (void *)&c);

    il = ql; i = 0;
    CuAssertIntEquals(tc, ql_true, ql_find(&il, &i, (void *)&a, cb_match_int));
    CuAssertIntEquals(tc, 0, i);

    ql_advance(&il, &i, 1);
    CuAssertIntEquals(tc, ql_true, ql_find(&il, &i, (void *)&a, cb_match_int));
    CuAssertIntEquals(tc, 2, i);

    ql_advance(&il, &i, 1);
    CuAssertIntEquals(tc, ql_false, ql_find(&il, &i, (void *)&a, cb_match_int));
    ql_free(ql);
}

static void test_find(CuTest *tc) {
    struct quicklist *ql = 0;
    struct quicklist *il;
    int i;

    ql_push(&ql, (void *)data);
    ql_push(&ql, (void *)(data+1));
    ql_push(&ql, (void *)data);

    il = ql; i = 0;
    CuAssertIntEquals(tc, ql_true, ql_find(&il, &i, (void *)data, 0));
    CuAssertIntEquals(tc, 0, i);

    ql_advance(&il, &i, 1);
    CuAssertIntEquals(tc, ql_true, ql_find(&il, &i, (void *)data, 0));
    CuAssertIntEquals(tc, 2, i);

    ql_advance(&il, &i, 1);
    CuAssertIntEquals(tc, ql_false, ql_find(&il, &i, (void *)data, 0));
    ql_free(ql);
}

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
  ql_free(ql);
}

static void test_empty_list(CuTest * tc)
{
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
  ql_free(ql);
}

static void test_replace(CuTest * tc)
{
  struct quicklist *ql = NULL;
  void * a;
  ql_push(&ql, (void *)data);
  a = ql_replace(ql, 0, (void *)(data+1));
  CuAssertPtrEquals(tc, (void *)data, a);
  CuAssertPtrEquals(tc, (void *)(data+1), ql_get(ql, 0));
  ql_free(ql);
}

int cmp_str(const void *lhs, const void *rhs) {
    return strcmp((const char *)lhs, (const char *)rhs);
}

static void test_set_insert_ex(CuTest * tc)
{
    const char * strings[] = {
        "aaa", "bbb", "ccc", "ddd"
    };
    struct quicklist *ql = NULL;
    int qi;
    
    /* insert a string: */
    CuAssertIntEquals(tc, ql_true, ql_set_insert_ex(&ql, (void *)strings[0], cmp_str));
    CuAssertIntEquals(tc, 1, ql_length(ql));

    /* insert a second string: */
    CuAssertIntEquals(tc, ql_true, ql_set_insert_ex(&ql, (void *)strings[1], cmp_str));
    CuAssertIntEquals(tc, 2, ql_length(ql));
    /* same string again, no change: */
    CuAssertIntEquals(tc, ql_false, ql_set_insert_ex(&ql, (void *)strings[1], cmp_str));
    CuAssertIntEquals(tc, 2, ql_length(ql));

    /* a third string */
    CuAssertIntEquals(tc, ql_true, ql_set_insert_ex(&ql, (void *)strings[2], cmp_str));
    CuAssertIntEquals(tc, 3, ql_length(ql));

    /* check that they are alphabetically ordered: */
    for (qi=0;qi!=3;++qi) {
        CuAssertPtrEquals(tc, (void *)strings[qi], ql_get(ql, qi));
    }
    
    CuAssertIntEquals(tc, 1, ql_set_find_ex(&ql, &qi, (void *)strings[2], cmp_str));
    CuAssertIntEquals(tc, 2, qi);
    ql_free(ql);
}

static void test_set_insert(CuTest * tc)
{
  struct quicklist *ql = NULL;
  int a, qi;
  a = ql_set_insert(&ql, (void *)data);
  CuAssertIntEquals(tc, 1, ql_length(ql));
  CuAssertIntEquals(tc, ql_true, a);
  a = ql_set_insert(&ql, (void *)(data+1));
  CuAssertIntEquals(tc, 2, ql_length(ql));
  CuAssertIntEquals(tc, ql_true, a);
  a = ql_set_insert(&ql, (void *)data);
  CuAssertIntEquals(tc, 2, ql_length(ql));
  CuAssertIntEquals(tc, ql_false, a);
  a = ql_set_insert(&ql, (void *)(data+2));
  CuAssertIntEquals(tc, ql_true, a);
  CuAssertIntEquals(tc, 3, ql_length(ql));
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, 0));
  CuAssertPtrEquals(tc, (void *)(data+1), ql_get(ql, 1));
  CuAssertPtrEquals(tc, (void *)(data+2), ql_get(ql, 2));

  a = ql_set_find(&ql, &qi, (void *)(data+3));
  CuAssertIntEquals(tc, 0, a);
  a = ql_set_find(&ql, &qi, (void *)data);
  CuAssertIntEquals(tc, 1, a);
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, qi));
  ql_free(ql);
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
  CuAssertIntEquals(tc, ql_true, a);
  CuAssertPtrEquals(tc, q2, ql);
  CuAssertPtrEquals(tc, (void *)data, ql_get(ql, 0));
  CuAssertPtrEquals(tc, (void *)(data+2), ql_get(ql, 1));
  CuAssertIntEquals(tc, 2, ql_length(ql));

  a = ql_set_remove(&ql, (void *)(data+1));
  CuAssertIntEquals(tc, ql_false, a);
  CuAssertPtrEquals(tc, q2, ql);

  a = ql_set_remove(&ql, (void *)data);
  CuAssertIntEquals(tc, ql_true, a);
  a = ql_set_remove(&ql, (void *)(data+2));
  CuAssertIntEquals(tc, ql_true, a);
  CuAssertPtrEquals(tc, 0, ql);

  ql_free(ql);
}

static void test_set_find(CuTest * tc)
{
  struct quicklist *ql = 0, *q2;
  int a, qi;

  q2 = ql;
  a = ql_set_find(&q2, 0, (void *)data);
  CuAssertIntEquals(tc, ql_false, a);
  CuAssertPtrEquals(tc, ql, q2);

  for (a = 0; a != 32; ++a) {
    ql_set_insert(&ql, (void *)(data + a));
  }

  q2 = ql;
  a = ql_set_find(&q2, 0, (void *)(data + 31));
  CuAssertIntEquals(tc, ql_true, a);
  CuAssertPtrEquals(tc, ql, q2);

  q2 = ql;
  a = ql_set_find(&ql, &qi, (void *)data);
  CuAssertIntEquals(tc, ql_true, a);
  CuAssertIntEquals(tc, 0, qi);
  CuAssertPtrEquals(tc, ql, q2);

  q2 = ql;
  a = ql_set_find(&ql, &qi, (void *)(data + 31));
  CuAssertIntEquals(tc, ql_true, a);
  CuAssertPtrEquals(tc, (void *)(data + 31), ql_get(ql, qi));
  CuAssertTrue(tc, ql != q2);

  ql_free(ql);
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

  ql_free(ql);
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
  ql_free(ql);
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
  ql_free(ql);
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
  ql_free(ql);
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
  ql_free(ql);
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
  ql_free(ql);
}

void add_suite_quicklist(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_loop);
  SUITE_ADD_TEST(suite, test_iter_delete_all);
  SUITE_ADD_TEST(suite, test_more);
  SUITE_ADD_TEST(suite, test_foreach);
  SUITE_ADD_TEST(suite, test_find);
  SUITE_ADD_TEST(suite, test_find_cb);
  SUITE_ADD_TEST(suite, test_mapreduce);
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
  SUITE_ADD_TEST(suite, test_set_insert_ex);
  SUITE_ADD_TEST(suite, test_push_doesnt_invalidate_iterator);
}

int main(void)
{
  int result;
  CuString *output = CuStringNew();
  CuSuite *suite = CuSuiteNew();


  add_suite_quicklist(suite);

  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  result = suite->failCount;
  printf("%s\n", output->buffer);
  CuSuiteDelete(suite);
  CuStringDelete(output);
  return result;
}

