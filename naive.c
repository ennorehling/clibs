#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  const char ** strings;
  int a, i, n = 1000;

  if (argc>1) {
    n = atoi(argv[1]);
  }
  strings = (const char**)malloc(sizeof(const char *)*n);
  for(i=0;i!=n;++i) {
    char str[20];
    sprintf(str, "%d", i);
    strings[i] = strdup(str);
  }
  for (a=2;a<argc;++a) {
    const char * prefix = argv[a];
    int o = 0;
    size_t len = strlen(prefix);
    for(i=0;i!=n;++i) {
      if (strncmp(prefix, strings[i], len)==0) ++o;
    }
    printf("%s matches: %d\n", prefix, o);
  }
  return 0;
}
