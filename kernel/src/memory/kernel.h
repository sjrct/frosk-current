//
// kernel/src/memory/kernel.h
//

#ifndef MEMORY_KERNEL_H
#define MEMORY_KERNEL_H

#include <types.h>
#include "../stubout.h"

void setup_kernel_memory(void);

//#define kalloc(SIZE) _kalloc(SIZE); dprintf("kalloc(%x):%s:%d\n", SIZE, __FILE__, __LINE__)
//#define kfree(PTR) _kfree(PTR); dprintf("kfree(%p):%s:%d\n", PTR, __FILE__, __LINE__)
#define kalloc(SIZE) _kalloc(SIZE)
#define kfree(PTR) _kfree(PTR)

void * _kalloc(ulong);
void _kfree(void *);

#endif
