/*
 * Allocate much.
 */

#include "tests/lib.h"

int main() {
  char* data[10];
  int j;

  for (j = 0; j < 50; j++) {
    int i;
    /* Allocate and insert in different places. */
    for (i = 0; i < 10; i++) {
      data[i] = (char*) malloc(15000);
      data[i][i * 1003] = '!';
    }
    /* Check that it's still there. */
    for (i = 0; i < 10; i++) {
      if (data[i][i * 1003] != '!') {
        return 1;
      }
    }

    /* Free it all, but not in order.  If this is not done, Buenos runs out of
       memory. */
    free(data[0]);
    free(data[3]);
    free(data[1]);
    free(data[5]);
    free(data[9]);
    free(data[2]);
    free(data[7]);
    free(data[6]);
    free(data[8]);
    free(data[4]);
  }
  
  syscall_exit(0);
  return 0;
}
