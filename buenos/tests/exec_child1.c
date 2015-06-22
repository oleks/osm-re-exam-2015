/*
 * Userland exec test; child 1.
 */

#include "tests/lib.h"

static const char msg[] = "Hello from child 1; I'll return the value 99.\n";

int main() {
  syscall_write(1, msg, sizeof(msg));
  return 99;
}
