#include "tests/lib.h"

#define VOLUME "[disk]"

int main() {
  int ifds[2];
  int ofds[2];
  int efds[2];
  char buf[256];
  (void) syscall_pipe(ifds);
  (void) syscall_pipe(ofds);
  (void) syscall_pipe(efds);
  pid_t pid = syscall_fork();
  if (pid == 0) {
    syscall_dup(ifds[0], 0);
    syscall_dup(ofds[1], 1);
    syscall_dup(efds[1], 2);
    syscall_exec(VOLUME "pipe2");
  } 
  else {
    int n = syscall_read(ofds[0], buf, sizeof(buf));
    (void) syscall_join(pid);
    syscall_write(stdout, buf, n);
  }
  return 0;
}
