/* Barrier test with semaphores.
 *
 * Use two semaphores to create a barrier for two processes.  Use another
 * semaphore as a lock to read and write atomically.
 */

#include "tests/lib.h"

#define VOLUME "[disk]"

int main() {
  usr_sem_t *wait0, *wait1, *read_write_lock;
  uint32_t prog0, prog1;
  int ret0, ret1;

  puts("Create the semaphores.\n");
  wait0 = syscall_sem_open("wait0", 0);
  wait1 = syscall_sem_open("wait1", 0);
  read_write_lock = syscall_sem_open("rwlock", 1);
  read_write_lock = read_write_lock;

  puts("Run the children.\n");
  prog0 = syscall_exec(VOLUME "sem_barrier_p0");
  prog1 = syscall_exec(VOLUME "sem_barrier_p1");

  puts("Wait for them to finish.\n");
  ret0 = syscall_join(prog0);
  ret1 = syscall_join(prog1);
  printf("Children joined with return values %d and %d.\n", ret0, ret1);

  puts("Destroy the semaphores.\n");
  syscall_sem_destroy(wait0);
  syscall_sem_destroy(wait1);

  syscall_halt();
  return 0;
}
