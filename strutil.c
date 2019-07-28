#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "strutil.h"

long strtolh(const char *nptr, int base)
{
  char * end;
  long result;
  
  assert(nptr);
  result = strtol(nptr, &end, base);
  if (*end) {
    if (*end=='k') result*=1000;
    else if (*end=='m') result*=1000000;
    else if (*end=='g') result*=1000000000;
    else if (*end=='K') result*=0x400;
    else if (*end=='M') result*=0x100000;
    else if (*end=='g') result*=0x40000000;
  }
  return result;
}
