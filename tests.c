#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

void add_suite_format(CuSuite *suite);
void add_suite_selist(CuSuite *suite);
void add_suite_strings(CuSuite *suite);
void add_suite_strutil(CuSuite *suite);
void add_suite_critbit(CuSuite *suite);
void add_suite_l10n(CuSuite *suite);
void add_suite_cmdline(CuSuite *suite);

int main(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();
    int result;
    add_suite_format(suite);
    add_suite_l10n(suite);
    add_suite_strings(suite);
    add_suite_strutil(suite);
    add_suite_critbit(suite);
    add_suite_selist(suite);
    add_suite_cmdline(suite);
    
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);

    result = suite->failCount;
    CuStringDelete(output);
    CuSuiteDelete(suite);
    return result;
}
