#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "critbit.h"
#include "strtolh.h"

int count_cb(const void * match, const void * key, size_t keylen, void *result)
{
  long * ctr = (long*)result;
  ++*ctr;
  return 0;
}

int main(int argc, char** argv) {
  clock_t start, now;
  long multi=1, i, n = 1000;
  int a, verbose = 0;
  critbit_tree cb = { 0 };
  
  for (a=1;argc>a;++a) {
    if (strcmp(argv[a], "-v")==0) verbose = 1;
    else if (strcmp(argv[a], "-x")==0) multi = strtolh(argv[++a], 10);
    else {
      n = strtolh(argv[a], 10);
      break;
    }
  }
  start = clock();
  for (i=0;i!=n;++i) {
    char str[20];
    sprintf(str, "%ld", i);
    cb_insert_str(&cb, str);
  }
  now = clock();
  printf("init: %.2fs\n", (float)(now-start)/CLOCKS_PER_SEC);

  start = clock();
  for (++a;a<argc;++a) {
    const char * prefix = argv[a];
    long m;
    for (m=0;m!=multi;++m) {
      long o = 0;
      cb_foreach(&cb, prefix, strlen(prefix), count_cb, &o);
      if (m==0 && verbose) printf("%ld matches for prefix %s in %ld strings\n", o, prefix, n);
    }
  }
  now = clock();
  printf("find: %.2fs\n", (float)(now-start)/CLOCKS_PER_SEC);

  return 0;
}
