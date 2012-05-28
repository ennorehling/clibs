#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "critbit.h"
#include "strtolh.h"

int count_cb(const void * match, const void * key, size_t keylen, void *result)
{
  int * ctr = (int*)result;
  ++*ctr;
  return 0;
}

int main(int argc, char** argv) {
  clock_t start, now;
  long n = 1000;
  int a, verbose = 0;
  critbit_tree cb = { 0 };
  
  for (a=1;argc>a;++a) {
    if (strcmp(argv[a], "-v")==0) verbose = 1;
    else {
      n = strtolh(argv[a], 10);
      break;
    }
  }
  start = clock();
  while (n--) {
    char str[20];
    sprintf(str, "%ld", n);
    cb_insert_str(&cb, str);
  }
  now = clock();
  printf("init: %f\n", (float)(now-start)/CLOCKS_PER_SEC);

  start = clock();
  for (;a<argc;++a) {
    const char * prefix = argv[a];
    int o = 0;
    cb_foreach(&cb, prefix, strlen(prefix), count_cb, &o);
    if (verbose) printf("%s matches: %d\n", prefix, o);
  }
  now = clock();
  printf("find: %f\n", (float)(now-start)/CLOCKS_PER_SEC);

  return 0;
}
