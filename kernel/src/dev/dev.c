//
// kernel/src/dev/dev.c
//

#include <string.h>
#include "dev.h"
#include "pci.h"
#include "block/ata.h"
#include "../debug.h"
#include "../leftovers.h"

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

	// attempt to detect boot medium from disk drive parameters from BIOS
	if (leftovers.dd.size >= 0x42 && leftovers.dd.sig == 0xBEDD) {
		in = (bm_interface_t *)(ulong)leftovers.dd.if_path;
		de = (bm_device_t    *)(ulong)leftovers.dd.device_path;

		if (!memcmp(leftovers.dd.hostbus, "ISA", 3)) {
			root_dev = create_ata_pio(in->isa.base, de->atapi.slave << 4);
		} else {
			// TODO
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
		// TODO
		dprintf("fixme: cannot read jack shit from extended bios disk data\n");
	}
}
