#include "tests/lib.h"

int main() {
  char hello[] = "Hello world!\n";
  syscall_write(stdout, hello, sizeof(hello) - 1);
  return 0;
}
