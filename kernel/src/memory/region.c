//
// kernel/src/region/region.c
//

#include <common.h>
#include "pages.h"
#include "kernel.h"
#include "region.h"
#include "../debug.h"
#include "../paging.h"

STRUCT(region_ls);

struct region_ls {
	region * rgn;
	region_ls * next;
};

static region_ls * in;

static block * create_block(ulong virt, ulong phys, ulong size)
{
	block * b;

	if (size > 0) {
		b = kalloc(sizeof(block));

		b->size = align(size, PAGE_SIZE);
		b->virt = virt;
		b->phys = phys;
		b->next = NULL;
	} else {
		b = NULL;
	}

	return b;
}

static block * alloc_block(ulong virt, ulong size)
{
	ulong phys;
	size = align(size, PAGE_SIZE);
	phys = alloc_pgs(size, PHYS_PAGES);
	return create_block(virt, phys, size);
}

region * construct(ulong virt, ulong phys, ulong size, int growup)
{
	region * r = kalloc(sizeof(region));

	size = align(size, PAGE_SIZE);

	r->size = size;
	r->virt = virt;
	r->first = create_block(virt, phys, size);
	r->last = r->first;
	r->growup = growup;

	return r;
}

region * allocate(ulong virt, ulong size, int growup)
{
	region * r = kalloc(sizeof(region));

	size = align(size, PAGE_SIZE);

	r->size = size;
	r->virt = virt;
	r->first = alloc_block(virt - !growup * size, size);
	r->last = r->first;
	r->growup = growup;

	return r;
}

void destroy(region * r)
{
	block * tmp;
	block * b = r->first;

	while (b != NULL) {
		free_pgs(b->phys, b->size, PHYS_PAGES);
		tmp = b->next;
		kfree(b);
		b = tmp;
	}
}

void grow(region * r, ulong to)
{
	long diff = to - r->virt;

	assert(r->growup || diff <= 0);
	assert(!r->growup || diff >= 0);

	if (!r->growup) diff = -diff;

	if (r->size < (ulong)diff) {
		r->size = align(diff, PAGE_SIZE);
	} else {
		assert(r->size < (ulong)diff);
	}
}

int in_region(region * r, ulong addr)
{
	return addr >= region_lower(r) && addr < region_upper(r);
}

int assure(region * r, ulong addr, ulong flags)
{
	block * b;

	if (!in_region(r, addr)) return 0;

	for (b = r->first; b != NULL; b = b->next) {
		if (b->virt <= addr && b->virt + b->size > addr) return 1;
	}

	b = alloc_block(floor(addr, PAGE_SIZE), PAGE_SIZE);
	pageto(b->virt, b->phys | flags);

	b->next = r->first;
	r->first = b;

	return 1;
}

static int overlap(region * x, region * y)
{
	ulong lx = region_lower(x);
	ulong ly = region_lower(y);
	ulong ux = region_upper(x);
	ulong uy = region_lower(y);
	return lx < uy && ux > ly;
}

int swapin(region * r, ulong flags)
{
	ulong offset;
	block * b;
	region_ls * ls;

	for (ls = in; ls != NULL; ls = ls->next) {
		if (overlap(r, ls->rgn)) return 0;
	}

	ls = kalloc(sizeof(region_ls));
	ls->next = in;
	ls->rgn = r;
	in = ls;

	for (b = r->first; b != NULL; b = b->next) {
		for (offset = 0; offset < b->size; offset += PAGE_SIZE) {
			pageto(
				b->virt + offset,
				(b->phys + offset) | flags
			);
		}
	}

	return 1;
}

void swapout(region * r)
{
	ulong offset;
	block * b;
	region_ls * ls = in;
	region_ls * pv = NULL;

	for (b = r->first; b != NULL; b = b->next) {
		for (offset = 0; offset < b->size; offset += PAGE_SIZE) {
			pageto(b->virt + offset, 0);
		}
	}

	while (ls != NULL && r != ls->rgn) {
		pv = ls;
		ls = ls->next;
	}

	if (ls != NULL) {
		if (pv == NULL) in = ls->next;
		else pv->next = ls->next;
		kfree(ls);
	} else {
		assert(ls != NULL);
	}
}

int swapflop(region * old, region * new, ulong flags)
{
	if (old != NULL) swapout(old);
	if (new != NULL) return swapin(new, flags);
	return 1;
}

void shift_rgn(region * r, ulong nv)
{
	block * b;
	ulong diff;

	diff = r->virt - nv;
	r->virt = nv;

	for (b = r->first; b != NULL; b = b->next) {
		b->virt -= diff;
	}
}

