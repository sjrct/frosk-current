/*
 * kernel/src/dev/video/video.c
 */

#include <string.h>
#include "video.h"
#include "../dev.h"
#include "../../asm.h"
#include "../../stubout.h"
#include "../../leftovers.h"

fs_entry_t * video_file;
fs_entry_t * cursor_file;

#define MODE_TEXT 1

struct cursor {
    int x;
    int y;
};

static struct cursor cursor_data;

qword cursor_read(byte *buf, qword addr, qword size, void *_unused) {
    if (addr >= sizeof(cursor_data))
        return 0;
    if (addr + size >= sizeof(cursor_data))
        size = sizeof(cursor_data) - addr;

    memcpy(buf, (byte *)&cursor_data, size);

    return size;
}

qword cursor_write(byte *buf, qword addr, qword size, void *_unused) {
    if (addr >= sizeof(cursor_data))
        return 0;
    if (addr + size >= sizeof(cursor_data))
        size = sizeof(cursor_data) - addr;

    memcpy((byte *)&cursor_data, buf, size);

    word pos = cursor_data.x | (cursor_data.y * leftovers.vmi.width);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (byte)(pos & 0xff));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (byte)((pos >> 8) & 0xff));

    return size;
}

void video_detect(void)
{
    dprintf("Screen dim: %dx%x\n", leftovers.vmi.width, leftovers.vmi.height);
    dprintf("Mode, segment = %x, %x:%x\n", leftovers.vmi.mode_attr, leftovers.vmi.wina_seg, leftovers.vmi.winb_seg);

	if (leftovers.vmi.mode_attr & MODE_TEXT) {
		/* text mode */
		video_file = fs_enter("tvb",
				(void *)(ulong)(leftovers.vmi.wina_seg * 0x10),
				leftovers.vmi.width * leftovers.vmi.height * 2,
				dev_dir
		);

        /* cursor file */
        cursor_file = fs_enter("cursor", NULL, 0, dev_dir);
        cursor_file->flags |= FS_ENT_HOOKED;
        cursor_file->u.file.read_hook = cursor_read;
        cursor_file->u.file.write_hook = cursor_write;
        cursor_file->u.file.size = sizeof(struct cursor);
	} else {
		/* TODO pixel mode */
	}
}

