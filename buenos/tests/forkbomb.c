#include "tests/lib.h"

/* Make a fork bomb!  This program must have syscall_fork return a negative
   number to indicate a fail, OR make 1024 processes. */

int main() {
  /* 2**10 should be plenty! */
  for (int i = 0; i < 10; i++) {
    pid_t pid = syscall_fork();
    if (pid < 0) {
      /* This will probably be printed multiple times, since multiple children
         will have tried to call `syscall_fork`. */
      puts("A negative value was returned!\n");
    }
  }
  return 0;
}
