#include <stdio.h>
#include <string.h>
#include <CuTest.h>

void add_suite_quicklist(CuSuite *suite);

int main(int argc, char ** argv)
{
  CuString *output = CuStringNew();
  CuSuite *suite = CuSuiteNew();

  add_suite_quicklist(suite);

  CuSuiteRun(suite);
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
  return suite->failCount;
}

