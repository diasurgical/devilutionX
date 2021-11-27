#include <stdio.h>

void bz_internal_error(int errcode) {
  fprintf(stderr, "BZip2 fatal error %d\n", errcode);
}
