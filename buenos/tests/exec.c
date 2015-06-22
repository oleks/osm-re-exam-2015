/*
 * Userland exec test.
 */

#include "tests/lib.h"

#define VOLUME "[disk]"

/* We use our own printing functions to keep it simple, but the code would've
   been prettier if we depended on library functions. */

/* Only works for constants and arrays, so watch out. */
#define write(s) syscall_write(1, s, sizeof(s))

void write_char(char c) {
  syscall_write(1, &c, 1);
}

void write_int(int n) {
  int k, leading_zeros, div;

  if (n < 0) {
    write("-");
    n *= -1;
  }

  if (n == 0) {
    write_char('0');
    return;
  }

  k = 1000000000;
  leading_zeros = 1;
  while (k > 0) {
    div = n / k;
    if (leading_zeros && div != 0) {
      leading_zeros = 0;
    }
    if (!leading_zeros) {
      write_char(div + '0');
    }
    n %= k;
    k /= 10;
  }
}

/* The programs to start. */
static const char child1_prog[] = VOLUME "exec_child1";
static const char child2_prog[] = VOLUME "exec_child2";

int main() {
  uint32_t child1, child2;
  int child1_ret, child2_ret;

  // Start the children.
  write("Starting program ");
  write(child1_prog);
  write("\n");
  child1 = syscall_exec(child1_prog);

  write("Starting program ");
  write(child2_prog);
  write("\n");
  child2 = syscall_exec(child2_prog);


  // Join them, but do it in reverse order to see if anything strange happens.
  write("Now joining child ");
  write_int(child2);
  write(" (");
  write(child2_prog);
  write(")\n");
  child2_ret = syscall_join(child2);
  write("Child joined with status: ");
  write_int(child2_ret);
  write("\n");

  write("Now joining child ");
  write_int(child1);
  write(" (");
  write(child1_prog);
  write(")\n");
  child1_ret = syscall_join(child1);
  write("Child joined with status: ");
  write_int(child1_ret);
  write("\n");

  return 0;
}
