#include <CuTest.h>
#include <stdio.h>
#include <string.h>

void add_suite_critbit(CuSuite *suite);

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();
    int result;
    add_suite_critbit(suite);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);

    result = suite->failCount;
    CuStringDelete(output);
    CuSuiteDelete(suite);
    return result;
}

