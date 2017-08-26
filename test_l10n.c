#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"
#include "l10n.h"

static void test_l10n(CuTest * tc)
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
    SUITE_ADD_TEST(suite, test_l10n_build);
}
