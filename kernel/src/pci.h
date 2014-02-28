//
// kernel/src/pci.h
//

#ifndef PCI_H
#define PCI_H

#include <types.h>

void pci_detect(void);
dword pci_cfg(byte, byte, byte, byte);
dword pci_read(dword);

#endif
