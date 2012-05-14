#include "critbit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  int e, n = 1000, o = 0;
  critbit_tree cb = { 0 };
  const char* results[4];
  const char * prefix = "";
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
  do {
    e = cb_find_prefix_str(&cb, prefix, results, 4, o);
    o += e;
  } while (e);
  printf("matches: %d\n", o);
  return 0;
}
