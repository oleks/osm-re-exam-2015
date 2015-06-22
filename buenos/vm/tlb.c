/*
 * TLB handling
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
 * $Id: tlb.c,v 1.6 2004/04/16 10:54:29 ttakanen Exp $
 *
 */

#include "kernel/panic.h"
#include "kernel/assert.h"
#include "vm/vm.h"
#include "vm/tlb.h"
#include "vm/pagetable.h"
#include "kernel/thread.h"
#include "lib/libc.h"

static void tlb_error(bool is_userland, char* msg)
{
  if (is_userland) {
    /* Finish the current offending process with a special return code.  This
       Buenos knows the difference between syscall-induced exceptions and
       interrupt-causing exceptions, and will not cause another interrupt, since
       now we're already in an interrupt. */
    kprintf(msg);
    kprintf("\n");
    process_finish(255);
  } else {
    KERNEL_PANIC(msg);
  }
}

void tlb_modified_exception(bool is_userland)
{
  tlb_error(is_userland, "TLB modified exception.");
}

/* Get valid bit from TLB entry corresponding to the given virtual address. */
static bool tlb_entry_is_valid(tlb_entry_t* entry, uint32_t vaddr)
{
  if (ADDR_IS_ON_ODD_PAGE(vaddr)) {
    return entry->V1;
  } else {
    return entry->V0;
  }
}

static void tlb_access_exception(bool is_userland)
{
  tlb_exception_state_t tes;
  _tlb_get_exception_state(&tes);

  pagetable_t *ptable = thread_get_current_thread_entry()->pagetable;
  if(ptable == NULL) {
    tlb_error(is_userland, "No pagetable associated with current thread.");
  }
  for (uint32_t i = 0; i < ptable->valid_count; i++) {
    tlb_entry_t *entry = &ptable->entries[i];
    if (entry->VPN2 == tes.badvpn2) {
      /* The virtual address matches a page's virtual address. */
      if (tlb_entry_is_valid(entry, tes.badvaddr)) {
        /* Place matching TLB entry somewhere in TLB. */
        _tlb_write_random(&ptable->entries[i]);
        return;
      }
      else {
        tlb_error(is_userland, "Found an invalid TLB entry while handling a TLB miss exception.");
      }
    }
  }
  KERNEL_PANIC("Page not found in pagetable.");
}

void tlb_load_exception(bool is_userland)
{
  tlb_access_exception(is_userland);
}

void tlb_store_exception(bool is_userland)
{
  tlb_access_exception(is_userland);
}
