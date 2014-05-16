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
ulong __syscall4(ulong, ulong, ulong, ulong, ulong);

enum {
	__FS_AQUIRE = 0,
	__FS_RELEASE,
	__FS_FIRST,
	__FS_NEXT,
	__FS_MKDIR,
	__FS_MKVDIR,
	__FS_SIZE,
	__FS_READ,
	__FS_WRITE,
};

typedef ulong file_t;

static inline file_t fs_aquire(const char * name, file_t par) {
	return __syscall2((ulong)name, par, __FS_AQUIRE);
}

static inline void fs_release(file_t f) {
	__syscall1(f, __FS_RELEASE);
}

static inline file_t fs_first(file_t dir) {
	return __syscall1(dir, __FS_FIRST);
}

static inline file_t fs_next(file_t prev) {
	return __syscall1(prev, __FS_NEXT);
}

static inline file_t fs_mkdir(const char * name, file_t par) {
	return __syscall2((ulong)name, par, __FS_MKDIR);
}

static inline file_t fs_mkvdir(const char * name, file_t par) {
	return __syscall2((ulong)name, par, __FS_MKVDIR);
}

static inline ulong fs_size(file_t f) {
	return __syscall1(f, __FS_SIZE);
}

static inline ulong fs_read(byte * buf, ulong addr, ulong size, file_t f) {
	return __syscall4((ulong)buf, addr, size, f, __FS_READ);
}

static inline ulong fs_write(byte * buf, ulong addr, ulong size, file_t f) {
	return __syscall4((ulong)buf, addr, size, f, __FS_WRITE);
}

#endif
