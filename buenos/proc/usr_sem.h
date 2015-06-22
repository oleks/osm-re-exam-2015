#ifndef BUENOS_PROC_SEMAPHORE
#define BUENOS_PROC_SEMAPHORE

#include "lib/types.h"
#include "kernel/semaphore.h"


typedef void usr_sem_t;

#define MAX_USR_SEM 32
#define MAX_USR_SEM_NAME 32

#define USR_SEM_ERROR_NOT_IN_USE -1

typedef enum {
  USR_SEM_FREE,
  USR_SEM_USED
} usr_sem_state_t;

typedef struct {
  usr_sem_state_t state;
  char name[MAX_USR_SEM_NAME];
  semaphore_t* kernel_sem;
} usr_sem_block_t;

void usr_sem_init();

usr_sem_t* usr_sem_open(const char* name, int value);

int usr_sem_p(usr_sem_t* sem);

int usr_sem_v(usr_sem_t* sem);

int usr_sem_destroy(usr_sem_t* sem);

#endif
