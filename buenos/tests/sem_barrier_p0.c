#include "tests/lib.h"

#define BUFFER_SIZE 60

int main() {
  usr_sem_t *wait0, *wait1, *read_write_lock;
  char line[BUFFER_SIZE];

  puts("[sem_barrier_p0] Open the semaphores.\n");
  wait0 = syscall_sem_open("wait0", -1);
  wait1 = syscall_sem_open("wait1", -1);
  read_write_lock = syscall_sem_open("rwlock", -1);

  puts("[sem_barrier_p0] Do work before barrier.\n");
  syscall_sem_p(read_write_lock);
  readline(line, BUFFER_SIZE);
  printf("[sem_barrier_p0] You wrote: %s\n", line);
  syscall_sem_v(read_write_lock);

  puts("[sem_barrier_p0] Wait for the other process.\n");
  syscall_sem_p(wait1); /* DIFFERENT THAN IN sem_barrier_prog1 */
  syscall_sem_v(wait0); /* DIFFERENT THAN IN sem_barrier_prog1 */

  puts("[sem_barrier_p0] Do work after barrier.\n");

  return 10;
}
