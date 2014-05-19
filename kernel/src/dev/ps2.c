/*
 * kernel/src/dev/ps2.c
 */

#include "ps2.h"
#include "dev.h"
#include "../lock.h"
#include "../debug.h"
#include "../interrupt.h"

#define KB_BUFFER_SIZE 1024

byte kb_buffer[KB_BUFFER_SIZE];
fs_entry_t * kb_buffer_file = NULL;

void kb_put(byte sc)
{
	static qword kb_count = 0;
	dprintf("sc = %d\n", sc);

	assert(kb_buffer_file != NULL);
	getlock(&kb_buffer_file->lock);
	dputs("fdsa\n");

	kb_buffer[kb_count % KB_BUFFER_SIZE] = sc;
	kb_buffer_file->u.file.size = ++kb_count;

	unlock(&kb_buffer_file->lock);
	dputs("fdsa\n");
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
