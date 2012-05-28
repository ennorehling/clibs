#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "strtolh.h"

int main(int argc, char** argv) {
  clock_t start, now;
  const char ** strings;
  long a, i, n = 1000;

  if (argc>1) {
    n = strtolh(argv[1], 10);
  }
  start = clock();
  strings = (const char**)malloc(sizeof(const char *)*n);
  for(i=0;i!=n;++i) {
    char str[20];
    sprintf(str, "%ld", i);
    strings[i] = strdup(str);
  }
  now = clock();
  printf("init: %f\n", (float)(now-start)/CLOCKS_PER_SEC);

  start = clock();
  for (a=2;a<argc;++a) {
    const char * prefix = argv[a];
    int o = 0;
    size_t len = strlen(prefix);
    for(i=0;i!=n;++i) {
      if (strncmp(prefix, strings[i], len)==0) ++o;
    }
    printf("%s matches: %d\n", prefix, o);
  }
  now = clock();
  printf("find: %f\n", (float)(now-start)/CLOCKS_PER_SEC);
  return 0;
}
