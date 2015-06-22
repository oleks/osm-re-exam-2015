#include "tests/lib.h"

/* Cause a TLB exception that can't be handled properly! */

int main() {
  int* t = (int*) 0;
  *t = 23;
  return 0;
}
