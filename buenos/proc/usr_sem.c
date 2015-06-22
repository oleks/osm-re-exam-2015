#include "proc/usr_sem.h"
#include "kernel/spinlock.h"
#include "kernel/semaphore.h"
#include "kernel/interrupt.h"
#include "lib/libc.h"

static usr_sem_block_t usr_sem_table[MAX_USR_SEM];
static spinlock_t usr_sem_table_slock;

static void reset(int i) {
  usr_sem_table[i].state = USR_SEM_FREE;
}

static usr_sem_block_t* find_sem(usr_sem_t* p) {
  usr_sem_block_t* sem = (usr_sem_block_t*) p;
  if (sem >= usr_sem_table
      && sem < usr_sem_table + (sizeof(usr_sem_table) / sizeof(usr_sem_block_t))) {
    return sem;
  }
  else {
    return NULL;
  }
}

void usr_sem_init() {
  spinlock_reset(&usr_sem_table_slock);

  int i;
  for (i = 0; i < MAX_USR_SEM; i++) {
    reset(i);
  }
}

usr_sem_t* usr_sem_open(const char* name, int value) {
  int i;
  interrupt_status_t intr_status;
  usr_sem_t* ret;

  intr_status = _interrupt_disable();
  spinlock_acquire(&usr_sem_table_slock);

  if (value < 0) {
    /* Try to find an existing semaphore. */
    for (i = 0; i < MAX_USR_SEM; i++) {
      usr_sem_block_t* sem = &usr_sem_table[i];
      if (sem->state == USR_SEM_USED
          && stringcmp(sem->name, name) == 0) {
        ret = (usr_sem_t*) sem;
        goto unlock;
      }
    }
    ret = NULL;
    goto unlock;
  }
  else {
    /* Try to create a fresh semaphore. */

    /* Check if one already exists. */
    for (i = 0; i < MAX_USR_SEM; i++) {
      usr_sem_block_t* sem = &usr_sem_table[i];
      if (sem->state == USR_SEM_USED
          && stringcmp(sem->name, name) == 0) {
        ret = NULL;
        goto unlock;
      }
    }

    /* Create the new one. */
    for (i = 0; i < MAX_USR_SEM; i++) {
      usr_sem_block_t* sem = &usr_sem_table[i];
      if (sem->state == USR_SEM_FREE) {
        sem->state = USR_SEM_USED;
        stringcopy(sem->name, name, MAX_USR_SEM_NAME);
        sem->kernel_sem = semaphore_create(value);
        ret = (usr_sem_t*) sem;
        goto unlock;
      }
    }

    /* No space left. */
    ret = NULL;
    goto unlock;
  }
 unlock:
  spinlock_release(&usr_sem_table_slock);
  _interrupt_set_state(intr_status);

  return ret;
}

int usr_sem_p(usr_sem_t* p) {
  usr_sem_block_t* sem = find_sem(p);
  if (sem == NULL || sem->state != USR_SEM_USED) {
    return USR_SEM_ERROR_NOT_IN_USE;
  }
  semaphore_P(sem->kernel_sem);
  return 0;
}

int usr_sem_v(usr_sem_t* p) {
  usr_sem_block_t* sem = find_sem(p);
  if (sem == NULL || sem->state != USR_SEM_USED) {
    return USR_SEM_ERROR_NOT_IN_USE;
  }
  semaphore_V(sem->kernel_sem);
  return 0;
}

int usr_sem_destroy(usr_sem_t* p) {
  usr_sem_block_t* sem = find_sem(p);
  if (sem == NULL || sem->state != USR_SEM_USED) {
    return 0;
  }
  int blocked = sem->kernel_sem->value < 0;
  if (blocked) {
    return 0;
  }
  else {
    /* Important: There is a race condition here, since the semaphore may
       have become blocking since we checked it.  The only way to avoid this
       race condition is to disable blocking on this semaphore altogether,
       but that can only be done if we keep the semaphore alive forever. */
    semaphore_destroy(sem->kernel_sem);
    sem->state = USR_SEM_FREE;
    return 1;
  }
}
