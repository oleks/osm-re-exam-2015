#include "tests/lib.h"

/* Test that fork works.  Print a lot as potential debugging help. */

#define CHILD_RETVAL 100
#define CHILD_RETVAL_ERROR 1
#define CHAR_ARRAY_LENGTH 9700
#define CHAR_POSITION 5533
#define SOME_CHAR 'u'
#define SOME_INT 501

/* Test of global memory. */
int some_number = SOME_INT;

int main() {
  /* Test of stack memory. */
  usr_sem_t* write_lock = syscall_sem_open("irrelevant", 1);

  /* Test of heap memory. */
  char* some_chars = malloc(sizeof(char) * CHAR_ARRAY_LENGTH);
  some_chars[CHAR_POSITION] = SOME_CHAR;

  usr_sem_t* parent_wait = syscall_sem_open("doesntmatter", 0);

  printf("Hello, I am %d.\n", syscall_getpid());

  pid_t pid = syscall_fork();
  if (pid < 0) {
    printf("syscall_fork failed with code %d\n", pid);
    return 1;
  }
  else if (pid == 0) {
    /* The new child process. */

    syscall_sem_p(write_lock);
    printf("Bwah bwah bwaaaah, I'm a child and my PID is %d.\n",
      syscall_getpid());
    printf("Hello from the child.  some_number = %d, some_char = %c\n",
           some_number, some_chars[CHAR_POSITION]);
    syscall_sem_v(write_lock);

    /* Test that TLB exceptions are handled. */
    int* alloc_test = malloc(19999 * sizeof(int));
    alloc_test[7721] = 119;
    if (alloc_test[7721] != 119) {
      puts("Error in TLB exception handling.\n");
      return CHILD_RETVAL_ERROR;
    }

    /* Modify the local copies, and signal the parent that it should check that
       their version hasn't been modified. */
    some_number++;
    some_chars[CHAR_POSITION]--;
    syscall_sem_v(parent_wait);

    syscall_sem_p(write_lock);
    printf("The child, updated.  some_number = %d, some_char = %c\n",
           some_number, some_chars[CHAR_POSITION]);
    syscall_sem_v(write_lock);

    return CHILD_RETVAL;
  }
  else {
    /* Still the same process. */

    /* Wait for the child to try to cheat the now-parent. */
    syscall_sem_p(parent_wait);

    syscall_sem_p(write_lock);
    printf("Hi, it's %d again, got this child %d.\n",
      syscall_getpid(), pid);
    printf("I'm a parent now.  some_number = %d, some_char = %c\n",
           some_number, some_chars[CHAR_POSITION]);
    syscall_sem_v(write_lock);
    int retval = syscall_join(pid);

    if (some_number != SOME_INT) {
      syscall_sem_p(write_lock);
      printf("Parent some_number %d does not match expected %d.\n",
             some_number, SOME_INT);
      syscall_sem_v(write_lock);
      return 2;
    }
    else if (some_chars[CHAR_POSITION] != SOME_CHAR) {
      syscall_sem_p(write_lock);
      printf("Parent some_char %d does not match expected %d.\n",
             some_chars[CHAR_POSITION], SOME_CHAR);
      syscall_sem_v(write_lock);
      return 3;
    }
    else if (retval != CHILD_RETVAL) {
      syscall_sem_p(write_lock);
      printf("Child return value %d does not match expected %d.\n",
             retval, CHILD_RETVAL);
      syscall_sem_v(write_lock);
      return 4;
    }
    else {
      puts("\nSUCCESS!\n\n");
      return 0;
    }
  }
}
