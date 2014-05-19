/*
 * kernel/src/dev/dev.c
 */

#include <string.h>
#include "dev.h"
#include "pci.h"
#include "ps2.h"
#include "block/ata.h"
#include "video/video.h"
#include "../debug.h"
#include "../leftovers.h"
#include "../memory/kernel.h"

device_t * root_dev = NULL;
fs_entry_t * dev_dir;

void init_devs(void)
{
	ulong edd_path;
	bm_interface_t * in;
	bm_device_t * de;
	bm_edd_t * edd;

	dev_dir = fs_mkvdir("dev", root_dir);

	pci_detect();
	video_detect();
	init_ps2_kb();

	/* attempt to detect boot medium from disk drive parameters from BIOS */
	if (leftovers.dd.size >= 0x42 && leftovers.dd.sig == 0xBEDD) {
		in = (bm_interface_t *)(ulong)leftovers.dd.if_path;
		de = (bm_device_t    *)(ulong)leftovers.dd.device_path;

		if (!memcmp(leftovers.dd.hostbus, "ISA", 3)) {
			root_dev = create_ata_pio(in->isa.base, de->atapi.slave << 4);
		} else {
			/* TODO */
			dprintf("fixme: detected pci boot medium\n");
		}
	}
	else if (leftovers.dd.size >= 0x1E) {
		edd_path = leftovers.dd.edd & 0xffff;
		edd_path += (leftovers.dd.edd >> 12) & 0xffff0;
		edd = (bm_edd_t *)edd_path;

		root_dev = create_ata_pio(edd->io_base, edd->flags & 0x10);
	}
	else {
		/* TODO */
		dprintf("fixme: cannot read jack shit from extended bios disk data\n");
	}

	fs_enter("root", root_dev, sizeof(*root_dev), dev_dir);
}

void dev_enable(device_t * d)
{
	if (d->enable != NULL) d->enable(d);
}

void dev_disable(device_t * d)
{
	if (d->disable != NULL) d->disable(d);
}

qword dev_read(device_t * d, void * buf, qword addr, qword size)
{
	ulong s, t;
	ulong osize = size;
	char * m = NULL;

	if (d->read == NULL || !size) return 0;

	/* Read data section not begining on 512-byte boundary, possibly not ending the same way */
	t = addr % d->block_size;
	if (addr % d->block_size) {
		if (m == NULL) m = kalloc(d->block_size);
		if (!d->read(d, m, addr / d->block_size, 1)) goto ret;

		s = d->block_size - t;
		s = s < size ? s : size;
		memcpy(buf, m + t, s);
		size -= s;
		addr += s;
		buf += s;
	}

	assert(!(addr % d->block_size) || !size);

	/* Read aligned section */
	s = size / d->block_size;
	if (s) {
		/* FIXME The return on failure here is not correct, though this is relativly trivial */
		if (d->read(d, buf, addr / d->block_size, s) != s) goto ret;
		addr += s * d->block_size;
		buf  += s * d->block_size;
		size -= s * d->block_size;
	}

	assert(!(addr % d->block_size) || !size);
	assert(size < d->block_size);

	/* Read section that ends not on boundary */
	if (size) {
		if (m == NULL) m = kalloc(d->block_size);
		if (!d->read(d, m, addr / d->block_size, 1)) goto ret;
		memcpy(buf, m, size);
		size -= size;
	}

ret:
	if (m != NULL) kfree(m);
	return osize - size;
}
