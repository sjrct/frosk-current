//
// kernel/src/paging.c
//

#include <util.h>
#include <common.h>
#include "asm.h"
#include "debug.h"
#include "paging.h"
#include "memory/pages.h"

#define ERROR_PRESENT  0x01
#define ERROR_WRITE    0x02
#define ERROR_USER     0x04
#define ERROR_RESERVED 0x08
#define ERROR_EXECUTE  0x10

#define PG_SWP_ENT (PM_L1_LOC + PG_SWP_LOC / 0x200)

static ulong get_entry(ulong addr, ulong tbl, int shf)
{
#ifdef __ARCH_X86_64
	#define LSHIFT 3
#else
	#define LSHIFT 2
#endif

	return tbl + (((addr >> shf) << LSHIFT) & 0xfff);

#undef LSHIFT
}

static void down(ulong addr, ulong tbl, int shift)
{
	long i;
	ulong entry = get_entry(addr, tbl, shift);
	ulong v = atq(entry);

	if (!(v & 1)) {
		v = alloc_pgs(PAGE_SIZE, PHYS_PAGES) | 7;

		atq(entry) = v;
		atq(PG_SWP_ENT) = v;
		invlpg(PG_SWP_LOC);

		for (i = 0; i < PAGE_SIZE; i++) atb(PG_SWP_LOC + i) = 0;
	} else {
		atq(PG_SWP_ENT) = v;
		invlpg(PG_SWP_LOC);
	}
}

void pageto(ulong virt, ulong value)
{
	assert(!(virt % PAGE_SIZE));
	invlpg(virt);

#ifdef __ARCH_X86_64
	if (virt >= HIGH_HALF_BOT) {
		virt -= NON_CANON_SIZE;
	}

	down(virt, PM_L4_LOC,  39);
	down(virt, PG_SWP_LOC, 30);
	down(virt, PG_SWP_LOC, 21);
#else
	down(virt, PM_L2_LOC, 22);
#endif
	atq(get_entry(virt, PG_SWP_LOC, 12)) = value;
}

ulong getpage(ulong virt)
{
	assert(!(virt % PAGE_SIZE));
	invlpg(virt);

#ifdef __ARCH_X86_64
	if (virt >= HIGH_HALF_BOT) {
		virt -= NON_CANON_SIZE;
	}

	down(virt, PM_L4_LOC,  39);
	down(virt, PG_SWP_LOC, 30);
	down(virt, PG_SWP_LOC, 21);
#else
	down(virt, PM_L2_LOC, 22);
#endif
	return get_entry(virt, PG_SWP_LOC, 12);
}
/*
void page_fault(ulong error, ulong cr2)
{
	region * heap;
	region * stack;
	kprintf("cr2 = %p\n", cr2);

	if (error & ERROR_PRESENT) {
		fatal("Page protection fault (%x) at %p", error, cr2);
	}

	if (~error & ERROR_USER) {
		fatal("Kernel page fault (%x) at %p", error, cr2);
	}

	assert(~error & ERROR_PRESENT);

	heap = cur_thrd->parent->code;
	stack = cur_thrd->stack;

	cr2 = floor(cr2, PAGE_SIZE);

	if (!assure(stack, cr2, 7) && !assure(heap, cr2, 7)) {
		if (cr2 < LOW_HALF_TOP / 2) {
			// presume heap
			grow(heap, cr2);
			assure(heap, cr2, 7);
		} else {
			// presume stack
			grow(stack, cr2);
			assure(stack, cr2, 7);
		}
	}
}
*/
