//
// kernel/src/pci.h
//

#ifndef PCI_H
#define PCI_H

#include <types.h>
#include "dev.h"

typedef struct pci_device {
    device_t * device;
    byte bus, dev, fun;
} pci_device_t;

void pci_detect(void);
dword pci_cfg(byte, byte, byte, byte);
dword pci_read(dword);

#endif
