#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "l10n.h"

static void test_l10n_heap(CuTest * tc)
{
    char buffer[32];
    l10n_text *txt;

    txt = l10n_text_create();
    CuAssertPtrNotNull(tc, txt);
    CuAssertIntEquals(tc, 1, txt->refcount);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "", buffer);
    l10n_text_assign(txt, "$name's blog (%num stars)", "$name", "Enno", "%num", 42, NULL);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "Enno's blog (42 stars)", buffer);
    txt = l10n_text_release(txt);
    CuAssertPtrEquals(tc, NULL, txt);
}

static void test_l10n(CuTest * tc)
{
    char buffer[32];
    l10n_text txt;

    l10n_text_init(&txt);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "", buffer);

    l10n_text_assign(&txt, "$name's blog (%num stars)", "$name", "Enno", "%num", 42, NULL);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "Enno's blog (42 stars)", buffer);

    l10n_text_assign(&txt, "Hodor", NULL);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "Hodor", buffer);

    l10n_text_assign(&txt, NULL);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "", buffer);

    l10n_text_free(&txt);
}

static void test_l10n_simple(CuTest * tc)
{
    char buffer[32];
    l10n_text txt = { 0 };

    l10n_text_assign(&txt, "$name's blog (%num stars)", "$name", "Enno", "%num", 42, NULL);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "Enno's blog (42 stars)", buffer);
    l10n_text_free(&txt);
}

static void test_l10n_const(CuTest * tc)
{
    char buffer[32];
    l10n_text txt = { "Hodor", 0 };

    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "Hodor", buffer);
    l10n_text_free(&txt);
}

static void test_l10n_nested(CuTest * tc)
{
    char buffer[32];
    l10n_text txt = { 0 };
    l10n_text *sub;

    sub = l10n_text_build("$name!", "$name", "Hodor", NULL);
    l10n_text_assign(&txt, "@name @name! @name!!", "@name", sub, NULL);
    CuAssertPtrEquals(tc, buffer, l10n_text_render(&txt, buffer, sizeof(buffer)));
    CuAssertStrEquals(tc, "Hodor! Hodor!! Hodor!!!", buffer);
    l10n_text_free(&txt);
    sub = l10n_text_release(sub);
    CuAssertPtrEquals(tc, NULL, sub);
}

static void test_l10n_build(CuTest * tc)
{
    char buffer[32];
    l10n_text *txt;

    txt = l10n_text_build("$name's blog (%num stars)", "$name", "Enno", "%num", 42, NULL);
    CuAssertPtrNotNull(tc, txt);
    CuAssertIntEquals(tc, 1, txt->refcount);
    l10n_text_render(txt, buffer, sizeof(buffer));
    CuAssertStrEquals(tc, "Enno's blog (42 stars)", buffer);
    txt = l10n_text_release(txt);
    CuAssertPtrEquals(tc, NULL, txt);
}

void add_suite_l10n(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_l10n);
    SUITE_ADD_TEST(suite, test_l10n_const);
    SUITE_ADD_TEST(suite, test_l10n_simple);
    SUITE_ADD_TEST(suite, test_l10n_nested);
    SUITE_ADD_TEST(suite, test_l10n_build);
    SUITE_ADD_TEST(suite, test_l10n_heap);
}
