/*
 * kernel/src/dev/disk/ata.c
 */

#include <util.h>
#include <common.h>
#include <string.h>
#include "ata.h"
#include "../dev.h"
#include "../../asm.h"
#include "../../debug.h"
#include "../../leftovers.h"
#include "../../memory/kernel.h"

#define SECTOR_SIZE   0x200
#define BUSY_BIT      0x80
#define READ_COMMAND  0x20
#define WRITE_COMMAND 0x30

typedef struct {
    byte is_slave;
    word base_port;
} source_t;

static void wait_busy(source_t * src)
{
    byte status;
    do {
        status = inb(src->base_port + 7);
    } while (status & BUSY_BIT);
}

static void ata_common(ulong lba, uint blcks, uchar cmd, source_t * src)
{
    outb(src->base_port + 6, 0xE0 | ((lba >> 24) & 0xf) | src->is_slave);
    outb(src->base_port + 1, 0);
    outb(src->base_port + 2, (uchar)blcks);
    outb(src->base_port + 3, (uchar)lba);
    outb(src->base_port + 4, (uchar)(lba >> 8));
    outb(src->base_port + 5, (uchar)(lba >> 16));

    outb(src->base_port + 7, cmd);

    /* delay ~400ns */
    io_wait();
    io_wait();
    io_wait();
    io_wait();
}

device_t * create_ata_pio(word bp, byte is)
{
    source_t * src = kalloc(sizeof(source_t));
    device_t * dev = kalloc(sizeof(device_t));

    src->base_port = bp;
    src->is_slave  = is;
    dev->data       = src;
    dev->enable     = NULL;
    dev->disable    = NULL;
    dev->read       = ata_pio_read;
    dev->write      = ata_pio_write;
    dev->block_size = SECTOR_SIZE;

    return dev;
}

/*
void stub_ata_init(void)
{
    ulong edd;
    bm_interface_t * intr;
    bm_device_t * dev;

    intr = (bm_interface_t *)leftovers.dd.if_path;
    dev  = (bm_device_t *)leftovers.dd.device_path;
    stub_sector_size = SECTOR_SIZE;

    if (leftovers.dd.edd == 0xffffffff) {
        base_port = intr->isa.base;
        is_slave  = dev->atapi.slave << 4;
    } else {
        edd = leftovers.dd.edd & 0xffff;
        edd += (leftovers.dd.edd >> 16) << 4;

        base_port = atw(edd);
        is_slave  = atb(edd + 4) & 0x10;
    }
}
*/
qword ata_pio_read(device_t * dev, void * data, qword offset, qword size)
{
    uint i, j;
    source_t * src = dev->data;

    if (!size) return 0;
    ata_common(offset, size, READ_COMMAND, src);

    for (i = 0; i < size; i++) {
        wait_busy(src);
        for (j = 0; j < 0x100; j++)  {
            ((ushort *)data)[i * 0x100 + j] = inw(src->base_port);
        }
    }

    return size;
}

qword ata_pio_write(device_t * dev, void * data, qword offset, qword size)
{
    uint i, j;
    source_t * src = dev->data;

    if (!size) return 0;
    ata_common(offset, size, WRITE_COMMAND, src);

    for (i = 0; i < size; i++) {
        wait_busy(src);
        for (j = 0; j < 0x100; j++)  {
            outw(src->base_port, ((ushort*)data)[i * 0x100 + j]);
        }
    }

    return size;
}

/* Maybe actually use this function ? */
/*
static int ata_identify(void)
{
    uchar v1, v2, p1, p2;

    // check for floating bus
    v1 = inb(active.base_port + 7);
    if (v1 == 0xff) return 0;

    // check that is ATA
    p1 = inb(active.base_port + 2);
    p2 = inb(active.base_port + 3);

    outb(active.base_port + 2, 0x12); // values are arbitrary
    outb(active.base_port + 3, 0x34);
    v1 = inb(active.base_port + 2);
    v2 = inb(active.base_port + 3);

    outb(active.base_port + 2, p1);
    outb(active.base_port + 3, p2);

    if (v1 != 0x12 || v2 != 0x34) return 0;

    // identify command
    // TODO

    return 1;
}
*/
