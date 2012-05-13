#include "critbit.h"
#include <stdio.h>

int main(int argc, char** argv) {
  int n = 10000;
  critbit_tree cb = { 0 };
  if (argc>1) {
    n = atoi(argv[1]);
  }
  while (n--) {
    char str[20];
    sprintf(str, "%d", n);
    cb_insert(&cb, str);
  }
  return 0;
}
