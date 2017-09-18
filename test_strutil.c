#include "strutil.h"
#include "CuTest.h"
#include <stdlib.h>

static void test_strtolh(CuTest *tc) {
    CuAssertIntEquals(tc, 1000, (int)strtolh("1k", 10));
    CuAssertIntEquals(tc, 1024, (int)strtolh("1K", 10));
    CuAssertIntEquals(tc, 14000, (int)strtolh("16k", 8));
}

void add_suite_strutil(CuSuite *suite)
{
  SUITE_ADD_TEST(suite, test_strtolh);
}
