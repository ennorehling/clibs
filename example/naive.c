#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "strtolh.h"

int main(int argc, char** argv) {
  clock_t start, now;
  const char ** strings;
  int a = 1, verbose = 0;
  long i, multi = 1, n = 1000;

  for (a=1;argc>a;++a) {
    if (strcmp(argv[a], "-v")==0) verbose = 1;
    else if (strcmp(argv[a], "-x")==0) multi = strtolh(argv[++a], 10);
    else {
      n = strtolh(argv[a], 10);
      break;
    }
  }
  start = clock();
  strings = (const char**)malloc(sizeof(const char *)*n);
  for(i=0;i!=n;++i) {
    char str[20];
    sprintf(str, "%ld", i);
    strings[i] = strdup(str);
  }
  now = clock();
  printf("init: %.2fs\n", (float)(now-start)/CLOCKS_PER_SEC);

  start = clock();
  for (++a;a<argc;++a) {
    const char * prefix = argv[a];
    long m;
    for (m=0;m!=multi;++m) {
      long o = 0;
      size_t len = strlen(prefix);
      for(i=0;i!=n;++i) {
        if (strncmp(prefix, strings[i], len)==0) ++o;
      }
      if (m==0 && verbose) printf("%ld matches for prefix %s in %ld strings\n", o, prefix, n);
    }
  }
  now = clock();
  printf("find: %.2fs\n", (float)(now-start)/CLOCKS_PER_SEC);
  for(i=0;i!=n;++i) {
      free(strings[i]);
  }
  free(strings);
  return 0;
}
