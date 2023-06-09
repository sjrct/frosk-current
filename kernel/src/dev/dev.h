//
// kernel/src/dev/dev.h
//

#ifndef DEV_DEV_H
#define DEV_DEV_H

#include <types.h>
#include <stddef.h>
#include "../fs/fs.h"

typedef struct device {
    void * data;
    void (* enable) (struct device *);
    void (* disable)(struct device *);
    qword (* read) (struct device *, void *, qword, qword);
    qword (* write)(struct device *, void *, qword, qword);
    ulong block_size;
} device_t;

extern device_t * root_dev;
extern fs_entry_t * dev_dir;

void init_devs(void);

void dev_enable(device_t *);
void dev_disable(device_t *);
qword dev_read(device_t *, void *, qword, qword);
qword dev_write(device_t *, void *, qword, qword);

#endif
