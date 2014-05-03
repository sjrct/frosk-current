//
// kernel/src/dev/disk/ata.h
//
// Current limitations ata driver:
//   28 bit lba
//   8 bit sector count
//

#ifndef IO_DISK_ATA_H
#define IO_DISK_ATA_H

#include <types.h>
#include "../dev.h"

device_t * create_ata_pio(word, byte);
qword ata_pio_read (device_t *, void *, qword, qword);
qword ata_pio_write(device_t *, void *, qword, qword);

#endif
