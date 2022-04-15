#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "selist.h"

static const char * lipsum = "Lorem ipsum dolor sit amet, "
"consectetur adipisicing elit, sed do eiusmod tempor "
"incididunt ut labore et dolore magna aliqua.";

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
    struct selist *ql = NULL;
    selist_push(&ql, (void *)lipsum);
    selist_push(&ql, (void *)lipsum);
    selist_map_reduce(ql, my_map, my_reduce, &buffer, &result);
    CuAssertIntEquals(tc, 152, result);
    selist_free(ql);
}

static bool callback_ctx(void *entry, void *x) {
    const char * str = (const char *)entry;
    int total = *(int *)x;
    total += str[0];
    *(int *)x = total;
    return total<76;
}

static int g_total;
static void callback(void *entry) {
    const char * str = (const char *)entry;
    g_total += str[0];
}

static void test_foreach(CuTest *tc) {
    int total = 0;
    struct selist *ql = NULL;
    selist_push(&ql, (void *)lipsum);
    selist_push(&ql, (void *)lipsum);
    selist_foreach_ex(ql, callback_ctx, &total);
    CuAssertIntEquals(tc, 76, total);
    selist_foreach(ql, callback);
    CuAssertIntEquals(tc, 152, g_total);
    selist_free(ql);
}

static int cb_match_int(const void*a, const void*b) {
    return *(const int *)a == *(const int *)b;
}

static void test_find_cb(CuTest *tc) {
    int a = 42;
    int b = 23;
    int c = 42;
    struct selist *ql = NULL;
    struct selist *il;
    int i;

    selist_push(&ql, (void *)&a);
    selist_push(&ql, (void *)&b);
    selist_push(&ql, (void *)&c);

    il = ql; i = 0;
    CuAssertTrue(tc, selist_find(&il, &i, (void *)&a, cb_match_int));
    CuAssertIntEquals(tc, 0, i);

    selist_advance(&il, &i, 1);
    CuAssertTrue(tc, selist_find(&il, &i, (void *)&a, cb_match_int));
    CuAssertIntEquals(tc, 2, i);

    selist_advance(&il, &i, 1);
    CuAssertTrue(tc, !selist_find(&il, &i, (void *)&a, cb_match_int));
    selist_free(ql);
}

static void test_find(CuTest *tc) {
    struct selist *ql = NULL;
    struct selist *il;
    int i;

    selist_push(&ql, (void *)lipsum);
    selist_push(&ql, (void *)(lipsum + 1));
    selist_push(&ql, (void *)lipsum);

    il = ql; i = 0;
    CuAssertTrue(tc, selist_find(&il, &i, (void *)lipsum, 0));
    CuAssertIntEquals(tc, 0, i);

    selist_advance(&il, &i, 1);
    CuAssertTrue(tc, selist_find(&il, &i, (void *)lipsum, 0));
    CuAssertIntEquals(tc, 2, i);

    selist_advance(&il, &i, 1);
    CuAssertTrue(tc, !selist_find(&il, &i, (void *)lipsum, 0));
    selist_free(ql);
}

static void test_insert(CuTest * tc)
{
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != 32; ++i) {
        CuAssertIntEquals(tc, i, selist_length(ql));
        selist_insert(&ql, 0, (void *)(lipsum + 31 - i));
    }
    for (i = 0; i != 32; ++i) {
        CuAssertPtrEquals(tc, (void *)(lipsum + i), selist_get(ql, i));
    }
    selist_free(ql);
}

static void test_empty_list(CuTest * tc)
{
    CuAssertIntEquals(tc, 0, selist_length(0));
    CuAssertTrue(tc, selist_empty(0));
}

static void test_insert_delete_gives_null(CuTest * tc)
{
    struct selist *ql = NULL;
    CuAssertIntEquals(tc, 1, selist_empty(ql));
    selist_push(&ql, (void *)lipsum);
    CuAssertIntEquals(tc, 0, selist_empty(ql));
    selist_delete(&ql, 0);
    CuAssertPtrEquals(tc, NULL, ql);
    selist_free(ql);
}

static void test_replace(CuTest * tc)
{
    struct selist *ql = NULL;
    void * a;
    selist_push(&ql, (void *)lipsum);
    a = selist_replace(ql, 0, (void *)(lipsum + 1));
    CuAssertPtrEquals(tc, (void *)lipsum, a);
    CuAssertPtrEquals(tc, (void *)(lipsum + 1), selist_get(ql, 0));
    selist_free(ql);
}


static void test_advance(CuTest * tc)
{
    struct selist *ql = NULL, *qli;
    int i, n = 31;
    for (i = 0; i != 32; ++i) {
        selist_insert(&ql, 0, (void *)(lipsum + i));
    }
    for (i = 0, qli = ql; qli; selist_advance(&qli, &i, 1), n--) {
        void * g = selist_get(qli, i);
        CuAssertPtrEquals(tc, (void *)(lipsum + n), g);
    }

    selist_free(ql);
}

static void test_push(CuTest * tc)
{
    struct selist *result, *ql = NULL;
    CuAssertIntEquals(tc, 0, selist_length(ql));
    result = selist_push(&ql, (void *)lipsum);
    CuAssertPtrEquals(tc, result, ql);
    CuAssertIntEquals(tc, 1, selist_length(ql));
    CuAssertPtrEquals(tc, (void *)lipsum, selist_get(ql, 0));
    selist_delete(&ql, 0);
    CuAssertPtrEquals(tc, NULL, ql);
}

static void test_push_returns_end(CuTest * tc)
{
    struct selist *result, *ql = NULL;
    int i = 0, qi = 0;
    do {
        result = selist_push(&ql, 0);
        i++;
    } while (result == ql);
    selist_advance(&ql, &qi, i - 1);
    CuAssertPtrEquals(tc, result, ql);
    selist_free(ql);
}

static void test_push_doesnt_invalidate_iterator(CuTest * tc)
{
    struct selist *list = NULL, *ql = NULL;
    int i, qi = 0;
    selist_push(&list, (void*)lipsum);
    ql = list;
    for (i = 0; i != 42; ++i) {
        void * n;
        n = selist_get(ql, qi);
        CuAssertPtrEquals(tc, (void *)(lipsum + i), n);
        selist_push(&list, (void *)(lipsum + (i * 2 + 1)));
        selist_advance(&ql, &qi, 1);
        selist_push(&list, (void *)(lipsum + (i * 2 + 2)));
    }
    selist_free(ql);
}

static void test_delete_edgecases(CuTest * tc)
{
    struct selist *ql = NULL;
    selist_delete(&ql, 0);
    CuAssertPtrEquals(tc, NULL, ql);
    selist_push(&ql, (void *)lipsum);
    selist_delete(&ql, -1);
    selist_delete(&ql, 32);
    CuAssertIntEquals(tc, 1, selist_length(ql));
    selist_free(ql);
}

static void test_insert_many(CuTest * tc)
{
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != 32; ++i) {
        selist_push(&ql, (void *)(lipsum + i));
    }
    for (i = 0; i != 32; ++i) {
        CuAssertIntEquals(tc, 32 - i, selist_length(ql));
        CuAssertPtrEquals(tc, (void *)(lipsum + i), selist_get(ql, 0));
        CuAssertPtrEquals(tc, (void *)(lipsum + 31), selist_get(ql, selist_length(ql) - 1));
        selist_delete(&ql, 0);
    }
    CuAssertPtrEquals(tc, NULL, ql);
    selist_free(ql);
}

static void test_delete_rand(CuTest * tc)
{
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != 32; ++i) {
        selist_push(&ql, (void *)(lipsum + i));
    }
    CuAssertIntEquals(tc, 32, selist_length(ql));
    selist_delete(&ql, 0);
    CuAssertPtrEquals(tc, (void *)(lipsum + 1), selist_get(ql, 0));
    CuAssertIntEquals(tc, 31, selist_length(ql));
    selist_delete(&ql, 30);
    CuAssertIntEquals(tc, 30, selist_length(ql));
    selist_free(ql);
}

void add_suite_selist(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_foreach);
    SUITE_ADD_TEST(suite, test_find);
    SUITE_ADD_TEST(suite, test_find_cb);
    SUITE_ADD_TEST(suite, test_mapreduce);
    SUITE_ADD_TEST(suite, test_advance);
    SUITE_ADD_TEST(suite, test_replace);
    SUITE_ADD_TEST(suite, test_push);
    SUITE_ADD_TEST(suite, test_push_returns_end);
    SUITE_ADD_TEST(suite, test_insert);
    SUITE_ADD_TEST(suite, test_empty_list);
    SUITE_ADD_TEST(suite, test_insert_delete_gives_null);
    SUITE_ADD_TEST(suite, test_insert_many);
    SUITE_ADD_TEST(suite, test_delete_rand);
    SUITE_ADD_TEST(suite, test_delete_edgecases);
    SUITE_ADD_TEST(suite, test_push_doesnt_invalidate_iterator);
}

