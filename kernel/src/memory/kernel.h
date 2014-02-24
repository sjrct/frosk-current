//
// kernel/src/memory/kernel.h
//

#ifndef MEMORY_KERNEL_H
#define MEMORY_KERNEL_H

#include <types.h>

void setup_kernel_memory(void);

void * kalloc(ulong);
void kfree(void *);

#endif
