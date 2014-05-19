/*
 * kernel/src/dev/ps2.h
 */

#ifndef DEV_PS2_H
#define DEV_PS2_H

#include <types.h>

void kb_put(byte);
qword kb_fs_read(byte *, qword, qword, void *);
void init_ps2_kb(void);

#endif
