//
// kernel/src/memory/pages.h
//

#ifndef MEMORY_PAGES_H
#define MEMORY_PAGES_H

#include <types.h>

#define PHYS_PAGES 0
#define VIRT_PAGES 1

#define INVALID_PAGE (-1)

void setup_pages(void);
ulong alloc_pgs(ulong, int);
void free_pgs(ulong, ulong, int);

#endif
