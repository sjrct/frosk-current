//
// kernel/src/pci.c
//

#include "dev.h"
#include "pci.h"
#include "../asm.h"
#include "../debug.h"
#include "../memory/kernel.h"

#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA 0xCFC

fs_entry_t * dev_pci_dir = NULL;

dword pci_addr(byte bus, byte dev, byte fun, byte off)
{
	return (1 << 31)
		|  ((dword)off & 0xFC)
		| (((dword)fun & 0x07) <<  8)
		| (((dword)dev & 0x1F) << 11)
		|  ((dword)bus         << 16);
}

dword pci_read(dword addr)
{
	outl(CONFIG_ADDR, addr);
	return inl(CONFIG_DATA);
}

static void assure(fs_entry_t ** d, dword x, fs_entry_t * par)
{
	int i;
	char str[3] = { '0', 0, 0 };

	if (*d == NULL) {dputc('\n');
		for (i = (x >= 0x10); i >= 0; --i, x >>= 4) {
			if ((x & 0xf) < 10) str[i] = '0' + (x & 0xf);
			else str[i] = 'a' + (x & 0xf) - 10;
		}

		*d = fs_mkvdir(str, par);
	}
}

static void add_pci_dev(dword bus, dword dev, dword fun, fs_entry_t * p)
{
	char str[2];
	pci_device_t * pci = kalloc(sizeof(pci_device_t));

	assert(bus < 0x100);
	assert(dev < 0x20);
	assert(fun < 8);

	pci->bus = bus;
	pci->dev = dev;
	pci->fun = fun;
	pci->device = NULL;

	str[0] = '0' + fun;
	str[1] = 0;
	fs_enter(str, pci, sizeof(pci_device_t), p);
}

void pci_detect(void)
{
	dword id;
	dword bus, dev, fun;
	fs_entry_t * db = NULL;
	fs_entry_t * dd = NULL;

	dev_pci_dir = fs_mkvdir("pci", dev_dir);

	for (bus = 0; bus < 256; bus++, db = NULL) {
		for (dev = 0; dev < 32; dev++, dd = NULL) {
			for (fun = 0; fun < 8; fun++) {
				id = pci_read(pci_addr(bus, dev, fun, 0));

				if ((id & 0xffff) != 0xffff) {
					dprintf("Found pci device: %d, %d, %d\n", bus, dev, fun);

					assure(&db, bus, dev_pci_dir);
					assure(&dd, dev, db);

					add_pci_dev(bus, dev, fun, dd);
				}
			}
		}
	}
}
