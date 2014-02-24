//
// kernel/src/memory/region.h
//

#ifndef MEMORY_REGION_H
#define MEMORY_REGION_H

#include <util.h>
#include <types.h>

STRUCT(block);
STRUCT(region);

struct block {
	ulong virt, phys, size;
	block * next;
};

struct region {
	ulong virt, size;
	block * first, * last;
	int growup;
};

#define region_lower(R) (R->growup ? R->virt : R->virt - R->size)
#define region_upper(R) (R->growup ? R->virt + R->size : R->virt)

region * construct(ulong, ulong, ulong, int);
region * allocate(ulong, ulong, int);
void destroy  (region *);
void grow     (region *, ulong);
int  in_region(region *, ulong);
int  assure   (region *, ulong, ulong);
int  swapin   (region *, ulong);
void swapout  (region *);
int  swapflop (region *, region *, ulong);
void shift_rgn(region *, ulong);

#endif
