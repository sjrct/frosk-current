/*
 * kernel/src/dev/ps2.c
 */

#include "ps2.h"
#include "dev.h"
#include "../lock.h"
#include "../debug.h"
#include "../interrupt.h"
#include "../scheduler.h"
#include "../memory/kernel.h"
#include "../memory/pages.h"

#define KB_BUFFER_SIZE 1024

#define ESCAPE 1

byte kb_buffer[KB_BUFFER_SIZE];
fs_entry_t * kb_buffer_file = NULL;
int kb_buffer_head = KB_BUFFER_SIZE - 1;

typedef struct ls {
	struct ls * next;
	byte sc;
} ls_t;

static ls_t * first = NULL;
static ls_t * last  = NULL;
static lock_t ls_lock = UNLOCKED;
static process_t * delay_proc = NULL;

static int kb_put_actual(byte sc)
{
	if (!trylock(&kb_buffer_file->lock)) return 0;

    kb_buffer_head = (kb_buffer_head + 1) % KB_BUFFER_SIZE;
	kb_buffer[kb_buffer_head] = sc;
	kb_buffer_file->u.file.size++;

	unlock(&kb_buffer_file->lock);

	return 1;
}

static void kb_put_delayed(void)
{
	ls_t * l;
    // FIXME

	for (;;) {
		if (first != NULL) {
			getlock(&ls_lock);

			l = first;
			first = l->next;
			if (first == NULL) last = NULL;

			unlock(&ls_lock);

			while (!kb_put_actual(l->sc));
			kfree(l);
		}
	}
}

void kb_put(byte sc)
{
	ls_t * l;
	assert(kb_buffer_file != NULL);

	if (!kb_put_actual(sc)) {
        // FIXME
        dprintf("delaying\n");
		l = kalloc(sizeof(ls_t));
		l->next = NULL;
		l->sc = sc;

		getlock(&ls_lock);
		if (first == NULL) {
			first = last = l;
		} else {
			last->next = l;
			last = l;
		}
		unlock(&ls_lock);

		if (delay_proc == NULL) {
			delay_proc = kspawn(kb_put_delayed);
		}
	}
}

void kb_handle(byte sc)
{
	void dump_threads(void), dump_procs(void);
	void dump_pgs(int type);

	if (sc == ESCAPE) {
		// Special case escape to get debug info
		asm volatile ("cli");
		dprintf("Hit escape\n\n");
		dump_threads();
		dprintf("\n");
		dump_procs();
		dprintf("\nPhysical pages:\n");
		dump_pgs(PHYS_PAGES);
		dprintf("\nVirtual pages:\n");
		dump_pgs(VIRT_PAGES);
		asm volatile ("sti");
	} else {
		kb_put(sc);
	}
}

qword kb_fs_read(byte * to, qword addr, qword size, void * unused)
{
    qword j;

    // FIXME use addr

	size = size > kb_buffer_file->u.file.size
        ? kb_buffer_file->u.file.size
        : size;

	for (j = 0; j < size; j++) {
        to[j] = kb_buffer[kb_buffer_head];
        kb_buffer_head = (kb_buffer_head - 1) % KB_BUFFER_SIZE;
	}

    kb_buffer_file->u.file.size -= size;

	return size;
}

void init_ps2_kb(void)
{
	void ps2_kb_irq(void);
	reg_irq(IRQ_PS2_KEYBOARD, ps2_kb_irq);

	kb_buffer_file = fs_enter("kb", NULL, 0, dev_dir);
	kb_buffer_file->flags |= FS_ENT_HOOKED;
	kb_buffer_file->u.file.read_hook = kb_fs_read;
	kb_buffer_file->u.file.write_hook = NULL;
}
