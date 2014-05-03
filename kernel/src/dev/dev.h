//
// kernel/src/dev/dev.h
//

#ifndef DEV_DEV_H
#define DEV_DEV_H

#include <types.h>
#include <stddef.h>
#include "../fs/fs.h"

typedef struct device {
	enum {
		DEVICE_BLOCK
	} type;

	void * data;
	void (* enable) (struct device *);
	void (* disable)(struct device *);

	union {
		struct {
			qword (* read) (struct device *, void *, qword, qword);
			qword (* write)(struct device *, void *, qword, qword);
			ulong block_size;
		} block;
	} inf;
} device_t;

extern device_t * root_dev;
extern fs_entry_t * dev_dir;

void init_devs(void);

#endif
