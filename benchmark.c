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
  int n = 1000, o = 0;
  critbit_tree cb = { 0 };
  const char *prefix = "";
  if (argc>1) {
    n = atoi(argv[1]);
    if (argc>2) {
      prefix = argv[2];
    }
  }
  while (n--) {
    char str[20];
    sprintf(str, "%d", n);
    cb_insert_str(&cb, str);
  }
  cb_foreach(&cb, prefix, strlen(prefix), count_cb, &o);
  printf("matches: %d\n", o);
  return 0;
}
