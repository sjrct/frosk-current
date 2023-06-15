/*
 * kernel/src/scheduler.c
 */

#include <common.h>
#include <string.h>
#include "debug.h"
#include "asm.h"
#include "lock.h"
#include "interrupt.h"
#include "scheduler.h"
#include "memory/pages.h"
#include "memory/kernel.h"

lock_t scheduler_lock = UNLOCKED;
thread_t * head_thrd = NULL;
process_t * head_proc = NULL;

void start_scheduler(void)
{
    void timer_irq();
    reg_irq(IRQ_TIMER, timer_irq);

    outb(0x43, 0x30);
    outb(0x40, 0x02);
    outb(0x40, 0x00);
}

static process_t *create_process(void) {
    process_t * p = kalloc(sizeof(process_t));

    p->first     = NULL;
    p->timeslice = 1000;
    init_mqueue(&p->mqueue);

    if (cur_thrd != NULL) {
        p->parent = cur_thrd->parent;
    } else {
        p->parent = NULL;
    }

    return p;
}

/* Add the process to the process list */
static void add_process(process_t *p) {
    getlock(&scheduler_lock);
    if (head_proc == NULL) {
        p->next = p;
        head_proc = p;
    } else {
        p->next = head_proc->next;
        head_proc->next = p;
    }
    unlock(&scheduler_lock);
}

process_t * uspawn(const byte * data, ulong csz, ulong bsz, ulong entry, int argc, const char ** argv)
{
    extern byte preamble_code;
    extern byte preamble_size;

    dprintf("uspawn(data=%p, csz=%X, bsz=%X, entry=%X, argc=%d, argv=%p)\n", data, csz, bsz, entry, argc, argv);

    int j, k;
    ulong i, n;
    ulong size;
    ulong tvirt, preamble;
    qword extv[3];
    char **argvt;
    process_t *p = create_process();

    size = csz + bsz;
    size += (ulong)&preamble_size;
    size = align(size, sizeof(char *));
    size += argc * sizeof(char *);
    for (j = 0; j < argc; j++) {
        size += strlen(argv[j]) + 1;
    }

    // Temporarily allocate virtual pages for the code region
    tvirt = alloc_pgs(size, VIRT_PAGES);
    p->code = allocate(tvirt, size, 1);

    swapin(p->code, 3);

    // Throughout this and the following sections, `i` points to the location in
    // temporary code region where we code in the code/rodata/argv etc
    for (i = tvirt; i < tvirt + csz; i++) {
        atb(i) = data[i - tvirt];
    }

    for (; i < tvirt + csz + bsz; i++) {
        atb(i) = 0;
    }

    // Copy the preamble
    preamble = i - tvirt + USPACE_BOT; /* To be used as the start point */
    for (n = 0; n < (ulong)&preamble_size; n++) {
        atb(i++) = (&preamble_code)[n];
    }

    // Copy argv
    i = align(i, sizeof(char *));
    argvt = (char **)i;
    i += argc * sizeof(char *);

    for (j = 0; j < argc; j++) {
        argvt[j] = (char *)(i - tvirt + USPACE_BOT);
        for (k = 0; argv[j][k] != 0; k++) {
            atb(i++) = argv[j][k];
        }
        atb(i++) = 0;
    }

    // Copied size == allocated size
    assert(i - tvirt == size);

    // Values to push onto the stack to be used by the preamble
    extv[0] = entry;
    extv[1] = (ulong)argvt - tvirt + USPACE_BOT;
    extv[2] = argc;

    swapout(p->code);
    shift_rgn(p->code, USPACE_BOT);

    free_pgs(tvirt, size, VIRT_PAGES);

    add_process(p);
    schedule(p, preamble, countof(extv), extv, 0);

    return p;
}

process_t * kspawn(void (* func)(void))
{
    process_t * p = create_process();

    p->argv = NULL;
    p->argv = 0;
    p->code = NULL;

    add_process(p);
    schedule(p, (ulong)func, 0, NULL, 1);

    return p;
}

thread_t * schedule(process_t * par, ulong start, int extz, const qword * extv, int kthrd)
{
    int i;
    qword tvirt;
    qword * ttop;
    qword * rtop = (qword *)(LOW_HALF_TOP - PAGE_SIZE);
    thread_t * t = kalloc(sizeof(thread_t));

    tvirt = alloc_pgs(2*PAGE_SIZE, VIRT_PAGES);
    t->stack  = allocate(tvirt + 2*PAGE_SIZE, 2*PAGE_SIZE, 0);
    t->state  = STATE_SETUP;
    t->parent = par;
    t->rsp    = (qword)&rtop[-6 - extz];
    t->page_fl = kthrd ? 3 : 7;

    swapin(t->stack, 3);

    ttop = (qword *)(tvirt + PAGE_SIZE);
    ttop[-6 - extz] = 1; /* Signal that no context data is pushed */
    ttop[-5 - extz] = start;
    ttop[-4 - extz] = kthrd ? KERN_CS : USER_CS | 3;
    ttop[-3 - extz] = getfl() | 0x200; /* TODO constant */
    ttop[-2 - extz] = (qword)(rtop - extz);
    ttop[-1 - extz] = kthrd ? KERN_DS : USER_DS | 3;

    for (i = 0; i < extz; i++) {
        ttop[-1 - i] = extv[i];
    }

    swapout(t->stack);
    shift_rgn(t->stack, LOW_HALF_TOP);
    free_pgs(tvirt, 2*PAGE_SIZE, VIRT_PAGES);

    getlock(&scheduler_lock);
    if (head_thrd == NULL) {
        t->next_sched = t;
        head_thrd = t;
    } else {
        t->next_sched = head_thrd->next_sched;
        head_thrd->next_sched = t;
    }
    unlock(&scheduler_lock);

    t->next_inproc = par->first;
    par->first = t;
    t->state = STATE_READY;

    return t;
}

void dump_threads(void) {
    char current;
    thread_t *t;

    if (trylock(&scheduler_lock) && head_thrd != NULL) {
        t = head_thrd;

        dprintf("Threads:\n");
        do {
            current = cur_thrd == t ? '*' : ' ';
            dprintf(" %c%p (proc: %p, rsp: %X, state: %x, page_fl: %x)\n",
                    current, t, t->parent, t->rsp, t->state, t->page_fl);
            t = t->next_sched;
        } while (t != head_thrd);

        unlock(&scheduler_lock);
    } else {
        dprintf("Scheduler locked\n");
    }
}

void dump_procs(void) {
    process_t *p;

    if (trylock(&scheduler_lock) && head_proc != NULL) {
        p = head_proc;

        dprintf("Processes:\n");
        do {
            dprintf("  %p\n", p);
            p = p->next;
        } while (p != head_proc);

        unlock(&scheduler_lock);
    } else {
        dprintf("Scheduler locked\n");
    }
}
