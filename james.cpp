#include <cstdio>
#include <ctime>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include "strtolh.h"

typedef std::vector< std::string > TStrVec;

int verbose = 0;

void init(TStrVec& strings, long n) {
  for (long i = 0; i != n; ++i) {
    char str[20];
    sprintf(str, "%ld", i);
    strings[i] = str;
  }
  std::sort(strings.begin(), strings.end());
}

long find(const TStrVec& strings, const std::string& prefix, long multi) {
  long o = 0;
  std::string prefix2(prefix);
  ++prefix2[prefix2.size() - 1];

  for (long m=0;m!=multi;++m) {
    TStrVec::const_iterator i = std::lower_bound(strings.begin(), strings.end(), prefix);
    TStrVec::const_iterator e = std::lower_bound(strings.begin(), strings.end(), prefix2);
    o += e-i;
  }
  return o;
}

int main(int argc, char ** argv) {
  clock_t start, now;
  long multi = 1, n = 1000;
  int a;

  for (a=1;argc>a;++a) {
    if (strcmp(argv[a], "-v")==0) verbose = 1;
    else if (strcmp(argv[a], "-x")==0) multi = strtolh(argv[++a], 10);
    else {
      n = strtolh(argv[a], 10);
      break;
    }
  }

  TStrVec strings(n);
  start = clock();
  init(strings, n);
  now = clock();
  printf("init: %.2fs\n", (float)(now-start)/CLOCKS_PER_SEC);

  start = clock();
  for (++a;a<argc;++a) {
    long o;
    std::string prefix(argv[a]);
    o = find(strings, prefix, multi);
    if (verbose) printf("%ld matches for prefix %s in %ld strings\n", o/multi, argv[a], n);
  }
  now = clock();
  printf("find: %.2fs\n", (float)(now-start)/CLOCKS_PER_SEC);

  return 0;
}
