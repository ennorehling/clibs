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
    struct selist *ql = 0;
    ql_push(&ql, (void *)lipsum);
    ql_push(&ql, (void *)lipsum);
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
    struct selist *ql = 0;
    ql_push(&ql, (void *)lipsum);
    ql_push(&ql, (void *)lipsum);
    ql_foreach(ql, my_callback);
    CuAssertIntEquals(tc, 152, gtotal);
    ql_foreachx(ql, my_callbackx, &total);
    CuAssertIntEquals(tc, 152, total);
    ql_free(ql);
}

static int cb_match_int(const void*a, const void*b) {
    return *(const int *)a == *(const int *)b;
}

static void test_find_cb(CuTest *tc) {
    int a = 42;
    int b = 23;
    int c = 42;
    struct selist *ql = 0;
    struct selist *il;
    int i;

    ql_push(&ql, (void *)&a);
    ql_push(&ql, (void *)&b);
    ql_push(&ql, (void *)&c);

    il = ql; i = 0;
    CuAssertTrue(tc, ql_find(&il, &i, (void *)&a, cb_match_int));
    CuAssertIntEquals(tc, 0, i);

    ql_advance(&il, &i, 1);
    CuAssertTrue(tc, ql_find(&il, &i, (void *)&a, cb_match_int));
    CuAssertIntEquals(tc, 2, i);

    ql_advance(&il, &i, 1);
    CuAssertTrue(tc, !ql_find(&il, &i, (void *)&a, cb_match_int));
    ql_free(ql);
}

static void test_find(CuTest *tc) {
    struct selist *ql = 0;
    struct selist *il;
    int i;

    ql_push(&ql, (void *)lipsum);
    ql_push(&ql, (void *)(lipsum + 1));
    ql_push(&ql, (void *)lipsum);

    il = ql; i = 0;
    CuAssertTrue(tc, ql_find(&il, &i, (void *)lipsum, 0));
    CuAssertIntEquals(tc, 0, i);

    ql_advance(&il, &i, 1);
    CuAssertTrue(tc, ql_find(&il, &i, (void *)lipsum, 0));
    CuAssertIntEquals(tc, 2, i);

    ql_advance(&il, &i, 1);
    CuAssertTrue(tc, !ql_find(&il, &i, (void *)lipsum, 0));
    ql_free(ql);
}

static void test_insert(CuTest * tc)
{
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != 32; ++i) {
        CuAssertIntEquals(tc, i, ql_length(ql));
        ql_insert(&ql, 0, (void *)(lipsum + 31 - i));
    }
    for (i = 0; i != 32; ++i) {
        CuAssertPtrEquals(tc, (void *)(lipsum + i), ql_get(ql, i));
    }
    ql_free(ql);
}

static void test_empty_list(CuTest * tc)
{
    CuAssertIntEquals(tc, 0, ql_length(0));
    CuAssertTrue(tc, ql_empty(0));
}

static void test_insert_delete_gives_null(CuTest * tc)
{
    struct selist *ql = NULL;
    CuAssertIntEquals(tc, 1, ql_empty(ql));
    ql_push(&ql, (void *)lipsum);
    CuAssertIntEquals(tc, 0, ql_empty(ql));
    ql_delete(&ql, 0);
    CuAssertPtrEquals(tc, 0, ql);
    ql_free(ql);
}

static void test_replace(CuTest * tc)
{
    struct selist *ql = NULL;
    void * a;
    ql_push(&ql, (void *)lipsum);
    a = ql_replace(ql, 0, (void *)(lipsum + 1));
    CuAssertPtrEquals(tc, (void *)lipsum, a);
    CuAssertPtrEquals(tc, (void *)(lipsum + 1), ql_get(ql, 0));
    ql_free(ql);
}


static void test_advance(CuTest * tc)
{
    struct selist *ql = NULL, *qli;
    int i, n = 31;
    for (i = 0; i != 32; ++i) {
        ql_insert(&ql, 0, (void *)(lipsum + i));
    }
    for (i = 0, qli = ql; qli; ql_advance(&qli, &i, 1), n--) {
        void * g = ql_get(qli, i);
        CuAssertPtrEquals(tc, (void *)(lipsum + n), g);
    }

    ql_free(ql);
}

static void test_push(CuTest * tc)
{
    struct selist *result, *ql = NULL;
    CuAssertIntEquals(tc, 0, ql_length(ql));
    result = ql_push(&ql, (void *)lipsum);
    CuAssertPtrEquals(tc, result, ql);
    CuAssertIntEquals(tc, 1, ql_length(ql));
    CuAssertPtrEquals(tc, (void *)lipsum, ql_get(ql, 0));
    ql_delete(&ql, 0);
    CuAssertPtrEquals(tc, 0, ql);
}

static void test_push_returns_end(CuTest * tc)
{
    struct selist *result, *ql = NULL;
    int i = 0, qi = 0;
    do {
        result = ql_push(&ql, 0);
        i++;
    } while (result == ql);
    ql_advance(&ql, &qi, i - 1);
    CuAssertPtrEquals(tc, result, ql);
    ql_free(ql);
}

static void test_push_doesnt_invalidate_iterator(CuTest * tc)
{
    struct selist *list = NULL, *ql = NULL;
    int i, qi = 0;
    ql_push(&list, (void*)lipsum);
    ql = list;
    for (i = 0; i != 42; ++i) {
        void * n;
        n = ql_get(ql, qi);
        CuAssertPtrEquals(tc, (void *)(lipsum + i), n);
        ql_push(&list, (void *)(lipsum + (i * 2 + 1)));
        ql_advance(&ql, &qi, 1);
        ql_push(&list, (void *)(lipsum + (i * 2 + 2)));
    }
    ql_free(ql);
}

static void test_delete_edgecases(CuTest * tc)
{
    struct selist *ql = NULL;
    ql_delete(&ql, 0);
    CuAssertPtrEquals(tc, 0, ql);
    ql_push(&ql, (void *)lipsum);
    ql_delete(&ql, -1);
    ql_delete(&ql, 32);
    CuAssertIntEquals(tc, 1, ql_length(ql));
    ql_free(ql);
}

static void test_insert_many(CuTest * tc)
{
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != 32; ++i) {
        ql_push(&ql, (void *)(lipsum + i));
    }
    for (i = 0; i != 32; ++i) {
        CuAssertIntEquals(tc, 32 - i, ql_length(ql));
        CuAssertPtrEquals(tc, (void *)(lipsum + i), ql_get(ql, 0));
        CuAssertPtrEquals(tc, (void *)(lipsum + 31), ql_get(ql, ql_length(ql) - 1));
        ql_delete(&ql, 0);
    }
    CuAssertPtrEquals(tc, 0, ql);
    ql_free(ql);
}

static void test_delete_rand(CuTest * tc)
{
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != 32; ++i) {
        ql_push(&ql, (void *)(lipsum + i));
    }
    CuAssertIntEquals(tc, 32, ql_length(ql));
    ql_delete(&ql, 0);
    CuAssertPtrEquals(tc, (void *)(lipsum + 1), ql_get(ql, 0));
    CuAssertIntEquals(tc, 31, ql_length(ql));
    ql_delete(&ql, 30);
    CuAssertIntEquals(tc, 30, ql_length(ql));
    ql_free(ql);
}

#if 0
static int cmp_str(const void *lhs, const void *rhs) {
    return strcmp((const char *)lhs, (const char *)rhs);
}

static void test_set_insert_ex(CuTest * tc)
{
    const char * strings[] = {
        "aaa", "bbb", "ccc", "ddd"
    };
    struct selist *ql = NULL;
    int qi;

    /* insert a string: */
    CuAssertTrue(tc, ql_set_insert_ex(&ql, (void *)strings[0], cmp_str));
    CuAssertIntEquals(tc, 1, ql_length(ql));

    /* insert a second string: */
    CuAssertTrue(tc, ql_set_insert_ex(&ql, (void *)strings[1], cmp_str));
    CuAssertIntEquals(tc, 2, ql_length(ql));
    /* same string again, no change: */
    CuAssertTrue(tc, !ql_set_insert_ex(&ql, (void *)strings[1], cmp_str));
    CuAssertIntEquals(tc, 2, ql_length(ql));

    /* a third string */
    CuAssertTrue(tc, ql_set_insert_ex(&ql, (void *)strings[2], cmp_str));
    CuAssertIntEquals(tc, 3, ql_length(ql));

    /* check that they are alphabetically ordered: */
    for (qi = 0; qi != 3; ++qi) {
        CuAssertPtrEquals(tc, (void *)strings[qi], ql_get(ql, qi));
    }

    CuAssertTrue(tc, ql_set_find_ex(&ql, &qi, (void *)strings[2], cmp_str));
    CuAssertIntEquals(tc, 2, qi);
    ql_free(ql);
}

static void test_set_insert_ex_large(CuTest * tc)
{
    const char * string = "12345678901234567890";
    struct selist *ql = NULL;
    int i;
    for (i = 0; i != QL_MAXSIZE + 1; ++i) {
        CuAssertTrue(tc, ql_set_insert_ex(&ql, (void *)(string + i), cmp_str));
    }
    CuAssertTrue(tc, !ql_set_insert_ex(&ql, (void *)(string + 3), cmp_str));
}

static void test_set_insert(CuTest * tc)
{
    struct selist *ql = NULL;
    int qi;

    CuAssertTrue(tc, ql_set_insert(&ql, (void *)lipsum));
    CuAssertIntEquals(tc, 1, ql_length(ql));
    CuAssertTrue(tc, ql_set_insert(&ql, (void *)(lipsum + 1)));
    CuAssertIntEquals(tc, 2, ql_length(ql));
    CuAssertTrue(tc, !ql_set_insert(&ql, (void *)lipsum));
    CuAssertIntEquals(tc, 2, ql_length(ql));

    CuAssertTrue(tc, ql_set_insert(&ql, (void *)(lipsum + 2)));
    CuAssertIntEquals(tc, 3, ql_length(ql));
    CuAssertPtrEquals(tc, (void *)lipsum, ql_get(ql, 0));
    CuAssertPtrEquals(tc, (void *)(lipsum + 1), ql_get(ql, 1));
    CuAssertPtrEquals(tc, (void *)(lipsum + 2), ql_get(ql, 2));

    CuAssertTrue(tc, !ql_set_find(&ql, &qi, (void *)(lipsum + 3)));
    CuAssertTrue(tc, ql_set_find(&ql, &qi, (void *)lipsum));
    CuAssertPtrEquals(tc, (void *)lipsum, ql_get(ql, qi));
    ql_free(ql);
}

static void test_set_remove(CuTest * tc)
{
    struct selist *ql = NULL, *q2;

    ql_set_insert(&ql, (void *)lipsum);
    ql_set_insert(&ql, (void *)(lipsum + 1));
    ql_set_insert(&ql, (void *)(lipsum + 2));

    q2 = ql;

    CuAssertTrue(tc, ql_set_remove(&ql, (void *)(lipsum + 1)));
    CuAssertPtrEquals(tc, q2, ql);
    CuAssertPtrEquals(tc, (void *)lipsum, ql_get(ql, 0));
    CuAssertPtrEquals(tc, (void *)(lipsum + 2), ql_get(ql, 1));
    CuAssertIntEquals(tc, 2, ql_length(ql));

    CuAssertTrue(tc, !ql_set_remove(&ql, (void *)(lipsum + 1)));
    CuAssertPtrEquals(tc, q2, ql);

    CuAssertTrue(tc, ql_set_remove(&ql, (void *)lipsum));
    CuAssertTrue(tc, ql_set_remove(&ql, (void *)(lipsum + 2)));
    CuAssertPtrEquals(tc, 0, ql);

    ql_free(ql);
}

static void test_set_find(CuTest * tc)
{
    struct selist *ql = 0, *q2;
    int a, qi;

    q2 = ql;
    a = ql_set_find(&q2, 0, (void *)lipsum);
    CuAssertIntEquals(tc, 0, a);
    CuAssertPtrEquals(tc, ql, q2);

    for (a = 0; a != 32; ++a) {
        ql_set_insert(&ql, (void *)(lipsum + a));
    }

    q2 = ql;
    a = ql_set_find(&q2, 0, (void *)(lipsum + 31));
    CuAssertIntEquals(tc, ql_true, a);
    CuAssertPtrEquals(tc, ql, q2);

    q2 = ql;
    a = ql_set_find(&ql, &qi, (void *)lipsum);
    CuAssertIntEquals(tc, ql_true, a);
    CuAssertIntEquals(tc, 0, qi);
    CuAssertPtrEquals(tc, ql, q2);

    q2 = ql;
    a = ql_set_find(&ql, &qi, (void *)(lipsum + 31));
    CuAssertIntEquals(tc, ql_true, a);
    CuAssertPtrEquals(tc, (void *)(lipsum + 31), ql_get(ql, qi));
    CuAssertTrue(tc, ql != q2);

    ql_free(ql);
}

static void test_iter_delete_all(CuTest * tc)
{
    selist * ql = 0;
    ql_iter iter;

    ql_push(&ql, (void *)lipsum);
    iter = qli_init(&ql);
    qli_delete(&iter);
    CuAssertPtrEquals(tc, 0, ql);
}

static void test_loop(CuTest * tc)
{
    ql_iter iter;
    selist * ql = 0;
    int result = 0;

    ql_push(&ql, (void *)lipsum);
    ql_push(&ql, (void *)(lipsum + 1));
    for (iter = qli_init(&ql); qli_more(iter);) {
        const char * c = (const char *)qli_next(&iter);
        result += *c;
    }
    CuAssertIntEquals(tc, lipsum[0] + lipsum[1], result);
}

static void test_more(CuTest * tc)
{
    ql_iter iter;
    selist * ql = 0;

    ql_push(&ql, (void *)lipsum);
    iter = qli_init(&ql);
    CuAssertTrue(tc, qli_more(iter));
    ql_delete(&iter.l, iter.i);
    CuAssertTrue(tc, !qli_more(iter));
}

static void test_insert_atend(CuTest * tc)
{
    struct selist *ql = NULL;
    ql_iter qi;
    int i;
    for (i = 0; i != QL_MAXSIZE; ++i) {
        ql_insert(&ql, i, (void *)(lipsum + i));
    }
    CuAssertIntEquals(tc, QL_MAXSIZE, ql_length(ql));
    qi = qli_init(&ql);
    for (i = 0; qli_more(qi); ++i) {
        void *ptr = qli_next(&qi);
        CuAssertPtrEquals(tc, (void *)(lipsum + i), ptr);
        if (i < QL_MAXSIZE - 1) {
            CuAssertPtrEquals(tc, ql, qi.l);
        }
        else {
            CuAssertPtrEquals(tc, 0, qi.l);
        }
    }
    ql_free(ql);
}

#endif

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
#if 0
    SUITE_ADD_TEST(suite, test_insert_atend);
    SUITE_ADD_TEST(suite, test_iter_delete_all);
    SUITE_ADD_TEST(suite, test_more);
    SUITE_ADD_TEST(suite, test_loop);
    SUITE_ADD_TEST(suite, test_set_remove);
    SUITE_ADD_TEST(suite, test_set_find);
    SUITE_ADD_TEST(suite, test_set_insert);
    SUITE_ADD_TEST(suite, test_set_insert_ex);
    SUITE_ADD_TEST(suite, test_set_insert_ex_large);
#endif
}

