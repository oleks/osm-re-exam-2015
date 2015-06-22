/*
 * Process startup.
 *
 * Copyright (C) 2003 Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef BUENOS_PROC_PROCESS_H
#define BUENOS_PROC_PROCESS_H

#include "kernel/config.h"
#include "lib/types.h"

#define USERLAND_STACK_TOP 0x7fffeffc

// Use name abstractions for errors instead of raw numbers.
#define PROCESS_PTABLE_FULL -1
#define PROCESS_ILLEGAL_JOIN -2
#define PROCESS_TTABLE_FULL -3

// All process data is stored in statically allocated memory because of kmalloc
// limitations, so we choose some sensible numbers.
#define PROCESS_MAX_PROCESSES 32
#define PROCESS_MAX_FILELENGTH 64

typedef int process_id_t;
typedef int openfile_t;

typedef enum {
  PROCESS_FREE,
  PROCESS_RUNNING,
  PROCESS_ZOMBIE
} process_state_t;

typedef struct {
  /* The executable name originates in userspace.  The struct instance may last
     longer than the process (so the original string memory might end up
     pointing to something else), so we have to copy the name to kernel
     memory. */
  char executable[PROCESS_MAX_FILELENGTH];

  /* We need to keep track of a process' state to make sure calls to eg. `join`
     do the right thing. */
  process_state_t state;

  /* A parent should be able to get a child's return value after it exits, so we
     store it here. */
  int retval;

  /* Only a process' parent should be able to interface with it. */
  process_id_t parent;

  /* End of the process heap. */
  uint32_t heap_end;

  /* The files opened by this process. */
  openfile_t files[CONFIG_MAX_OPEN_FILES];
} process_control_block_t;

void process_start(process_id_t pid);
void process_init();

/* Process management. */
process_id_t process_spawn(const char* executable);
void process_finish(int retval);
int process_join(process_id_t pid);
int process_fork();

/* Return PID of current process. */
process_id_t process_get_current_process();

/* Return PCB of current process. */
process_control_block_t *process_get_current_process_entry();

/* Memory allocation. */
uint32_t process_memlimit(uint32_t heap_end);

/* Process file bookkeeping. */
bool process_add_file(openfile_t file);
bool process_remove_file(openfile_t file);
bool process_has_open_file(openfile_t file);

#endif /* BUENOS_PROC_PROCESS_H */
