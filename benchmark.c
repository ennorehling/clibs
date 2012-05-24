#include "critbit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_cb(const void * match, const void * key, size_t keylen, void *result)
{
  int * ctr = (int*)result;
  ++*ctr;
  return 0;
}

int main(int argc, char** argv) {
  int a, n = 1000;
  critbit_tree cb = { 0 };
  if (argc>1) {
    n = atoi(argv[1]);
  }
  while (n--) {
    char str[20];
    sprintf(str, "%d", n);
    cb_insert_str(&cb, str);
  }
  for (a=2;a<argc;++a) {
    const char * prefix = argv[a];
    int o = 0;
    cb_foreach(&cb, prefix, strlen(prefix), count_cb, &o);
    printf("%s matches: %d\n", prefix, o);
  }
  return 0;
}
