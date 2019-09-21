#include "format.h"
#include "CuTest.h"

static char *list_english(int argc, const char **argv) {
    char buffer[128];
    return format_list(argc, argv, buffer, sizeof(buffer),
        "{0} and {1}",
        "{0}, {1}",
        "{0}, {1}",
        "{0} and {1}");
}

static void test_format(CuTest *tc) {
    const char *argv[] = { "apple", "banana", "mango", "pineapple" };
    
    CuAssertStrEquals(tc, "apple", list_english(1, argv));
    CuAssertStrEquals(tc, "apple and banana", list_english(2, argv));
    CuAssertStrEquals(tc, "apple, banana and mango", list_english(3, argv));
}

void add_suite_format(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_format);
}
