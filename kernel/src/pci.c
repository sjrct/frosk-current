//
// kernel/src/pci.c
//

#include "asm.h"
#include "pci.h"
#include "debug.h"

#define PCI_ADDR_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

dword pci_addr(byte bus, byte dev, byte fun, byte off)
{
	return (1 << 31)
		|  ((dword)off & 0xfc)
		| (((dword)fun & 0x07) <<  8)
		| (((dword)dev & 0x1f) << 11)
		| (((dword)bus & 0xff) << 16);
}

dword pci_read(dword addr)
{
	outl(PCI_ADDR_PORT, addr);
	return inl(PCI_DATA_PORT);
}

void pci_detect(void)
{
	dword id;
	byte bus, dev;

	for (bus = 0; bus < 256; bus++) {
		for (dev = 0; dev < 32; dev++) {
			id = pci_addr(bus, dev, 0, 0);

			if ((id & 0xffff) != 0xffff) {
				dprintf("Found device: %x\n", id);
			}
		}
	}
}
