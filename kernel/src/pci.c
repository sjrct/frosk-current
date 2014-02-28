//
// kernel/src/pci.c
//

#include "asm.h"
#include "pci.h"
#include "debug.h"

#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA 0xCFC

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

void pci_detect(void)
{
	dword id, cls;
	dword bus, dev;

	for (bus = 0; bus < 256; bus++) {
		for (dev = 0; dev < 32; dev++) {
			id = pci_read(pci_addr(bus, dev, 0, 0));
			cls = pci_read(pci_addr(bus, dev, 0, 8)); 

			if ((id & 0xffff) != 0xffff) {
				dprintf("Found device %d on bus %d: %x | %x\n", dev, bus, id, cls);
			}
		}
	}
}
