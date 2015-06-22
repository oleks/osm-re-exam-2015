/*
 * Userland exec test; child 2.
 */

#include "tests/lib.h"

static const char msg[] = "Hello from child 2; I'll return the value 5.\n";

int main() {
  syscall_write(1, msg, sizeof(msg));
  return 5;
}
