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

#define KB_BUFFER_SIZE 1024

byte kb_buffer[KB_BUFFER_SIZE];
fs_entry_t * kb_buffer_file = NULL;

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
	static qword kb_count = 0;

	if (!trylock(&kb_buffer_file->lock)) return 0;

	kb_buffer[kb_count % KB_BUFFER_SIZE] = sc;
	kb_buffer_file->u.file.size = ++kb_count;

	unlock(&kb_buffer_file->lock);

	return 1;
}

static void kb_put_delayed(void)
{
	ls_t * l;

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

qword kb_fs_read(byte * to, qword addr, qword size, void * unused)
{
	qword rem, i;

	if (addr > KB_BUFFER_SIZE) {
		return 0;
	} else {
		i = addr;
	}

	rem = KB_BUFFER_SIZE - i;
	rem = size > rem ? rem : size;

	for (; rem; --rem) {
		to[rem - 1] = kb_buffer[i];
		i = (i + 1) % KB_BUFFER_SIZE;
	}

	return size - rem;
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
