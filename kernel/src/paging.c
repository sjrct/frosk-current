//
// kernel/src/paging.c
//

#include <util.h>
#include <common.h>
#include "asm.h"
#include "debug.h"
#include "paging.h"
#include "scheduler.h"
#include "memory/pages.h"

#define ERROR_PROTECTION   0x01 // Set caused by a protection exception, unset by non-present page
#define ERROR_WRITE        0x02 // Caused by write if set, read if not
#define ERROR_USER         0x04 // Caused in user mode if set
#define ERROR_RESERVED     0x08 // Caused by reserved bit set
#define ERROR_EXECUTE      0x10 // Caused by an instruction fetch
#define ERROR_PROTECT_KEYS 0x20 // Caused by a protection key violation
#define ERROR_SHADOW_STACK 0x40 // Caused by shadow stack access
#define ERROR_HLAT         0x80 // Caused by HLAT paging issue
#define ERROR_SGX        0x8000 // Caused by Software Guard Extension

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

void print_region(const char *str, region_t *rgn) {
    dprintf("rgn %s(%d) : %X (%X)\n", str, rgn->growup, rgn->virt, rgn->size);

    block_t *b = rgn->first;
    while (b != NULL) {
        dprintf("-> %X->%X (%X)\n", b->virt, b->phys, b->size);
        b = b->next;
    }
}

void page_fault(ulong error, ulong cr2, ulong instr_ptr)
{
    region_t * heap;
    region_t * stack;
    ulong page;

    if (error == (ERROR_WRITE | ERROR_USER)) {
        /* Userspace, write, page-not-present error */
        /* (Shouldn't be reading from unalloced pages yet) */
        /* TODO swap to disk */

        page = floor(cr2, PAGE_SIZE);

        heap = cur_thrd->parent->code;
        stack = cur_thrd->stack;

        if (!assure(stack, page, 7) && !assure(heap, page, 7)) {
            if (page < LOW_HALF_TOP / 2) {
                // presume heap
                grow(heap, page);
                assure(heap, page, 7);
            } else {
                // presume stack
                grow(stack, page);
                assure(stack, page, 7);
            }
        }
    } else {
        dprintf("Unhandlable page fault: error=%X addr=%p", error, cr2);
        dprintf("At instruction %p\n", instr_ptr);
        for(;;);
    }
}
