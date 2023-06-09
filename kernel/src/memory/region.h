/*
 * kernel/src/memory/region.h
 */

#ifndef MEMORY_REGION_H
#define MEMORY_REGION_H

#include <util.h>
#include <types.h>
#include "../stubout.h"

typedef struct block block_t;
typedef struct region region_t;

struct block {
	ulong virt, phys, size;
	block_t * next;
};

struct region {
	ulong virt, size;
	block_t * first, * last;
	int growup;
};

#define region_lower(R) (R->growup ? R->virt : R->virt - R->size)
#define region_upper(R) (R->growup ? R->virt + R->size : R->virt)

region_t * construct(ulong, ulong, ulong, int);
region_t * allocate(ulong, ulong, int);
void destroy  (region_t *);
void grow     (region_t *, ulong);
int  in_region(region_t *, ulong);
int  assure   (region_t *, ulong, ulong);
int  swapin   (region_t *, ulong);
void swapout  (region_t *);
int  swapflop (region_t *, region_t *, ulong);
void shift_rgn(region_t *, ulong);

#endif
