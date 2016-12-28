#include "strutil.h"
#include "CuTest.h"
#include <stdlib.h>

void add_suite_strutil(CuSuite *suite);

static void test_strtolh(CuTest *tc) {
    CuAssertIntEquals(tc, 1000, (int)strtolh("1K", 10));
}

void add_suite_strutil(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_strtolh);
}
