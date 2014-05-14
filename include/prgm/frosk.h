/*
 * include/prgm/frosk.h
 * include/lib/frosk.h
 */

#ifndef _FROSK_H_
#define _FROSK_H_

#include <types.h>

ulong __syscall0(ulong);
ulong __syscall1(ulong, ulong);
ulong __syscall2(ulong, ulong, ulong);
ulong __syscall3(ulong, ulong, ulong, ulong);

enum {
	__DUMMY = 0
};

#endif
