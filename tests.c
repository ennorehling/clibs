#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

void add_suite_selist(CuSuite *suite);
void add_suite_strutil(CuSuite *suite);
void add_suite_critbit(CuSuite *suite);

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();
    int result;
    add_suite_strutil(suite);
    add_suite_critbit(suite);
    add_suite_selist(suite);
    
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);

    result = suite->failCount;
    CuStringDelete(output);
    CuSuiteDelete(suite);
    return result;
}

