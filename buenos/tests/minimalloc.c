/*
 * Allocate a little.
 */

#include "tests/lib.h"

int main() {
  char* data1;
  char* data3;

  data1 = (char*) malloc(1);
  data1[0] = '1';

  data3 = (char*) malloc(3);
  data3[0] = '3';
  data3[1] = '4';
  data3[2] = '5';

  free(data3);
  free(data1);
  
  syscall_exit(0);
  return 0;
}
