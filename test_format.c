#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include "format.h"
#include "CuTest.h"
#include <string.h>

static char *list_english(int argc, const char **argv) {
    static char buffer[128];
    return format_list(argc, argv, buffer, sizeof(buffer),
        "{0} and {1}",
        "{0}, {1}",
        "{0}, {1}",
        "{0} and {1}");
}

static void test_format_replace(CuTest * tc) {
    char result[64];
    format_replace("Hello {0}!", "{0}", "World", result, sizeof(result));
    CuAssertStrEquals(tc, "Hello World!", result);
}

static void test_format_replace_overlap(CuTest * tc) {
    char result[64];
    strcpy(result, "Hello {0}!");
    format_replace(result, "{0}", "World", result, sizeof(result));
    CuAssertStrEquals(tc, "Hello World!", result);

    strcpy(result, "World");
    format_replace("Hello {0}!", "{0}", result, result, sizeof(result));
    CuAssertStrEquals(tc, "Hello World!", result);
}

static void test_format_list(CuTest *tc) {
    const char *argv[] = { "apple", "banana", "mango", "pineapple" };

    CuAssertStrEquals(tc, "apple", list_english(1, argv));
    CuAssertStrEquals(tc, "apple and banana", list_english(2, argv));
    CuAssertStrEquals(tc, "apple, banana and mango", list_english(3, argv));
}

void add_suite_format(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_format_list);
    SUITE_ADD_TEST(suite, test_format_replace_overlap);
    SUITE_ADD_TEST(suite, test_format_replace);
}
