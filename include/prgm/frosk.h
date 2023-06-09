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
    __BEXEC = 0,
    __FEXEC,
	__FS_ACQUIRE,
	__FS_RELEASE,
	__FS_FIRST,
	__FS_NEXT,
	__FS_MKDIR,
	__FS_MKVDIR,
	__FS_SIZE,
	__FS_READ,
	__FS_WRITE,
	__PROC_SEND_MSG,
	__PROC_RECV_MSG,
	__PROC_CONSUME_MSG,
	__PROC_STOP_RECV,
};

/* FIXME These are actually kernel pointers, which is dangerous */
typedef ulong file_t;
typedef ulong pid_t;
typedef ulong next_message_t;

#define parent_pid ((pid_t)0)

static inline pid_t bexec(const byte * buf, ulong size, int argc, const char ** argv) {
    return __syscall4((ulong)buf, size, argc, (ulong)argv, __BEXEC);
}

static inline pid_t fexec(const char * fn, int argc, const char ** argv, file_t par) {
    return __syscall4((ulong)fn, argc, (ulong)argv, (ulong)par, __FEXEC);
}

static inline file_t fs_acquire(const char * name, file_t par) {
	return __syscall2((ulong)name, par, __FS_ACQUIRE);
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

static inline ulong fs_write(const byte * buf, ulong addr, ulong size, file_t f) {
	return __syscall4((ulong)buf, addr, size, f, __FS_WRITE);
}

static inline void proc_send_msg(const void * buf, ulong size, pid_t pid) {
	__syscall3((ulong)buf, size, pid, __PROC_SEND_MSG);
}

static inline ulong proc_recv_msg(void * buf, ulong size, next_message_t *next) {
	return __syscall3((ulong)buf, size, (ulong)next, __PROC_RECV_MSG);
}

static inline void proc_consume_msg(next_message_t *next) {
	__syscall1((ulong)next, __PROC_CONSUME_MSG);
}

static inline void proc_stop_recv(next_message_t *next) {
	__syscall1((ulong)next, __PROC_STOP_RECV);
}

static inline qword rdtsc(void) {
    dword hi, lo;
    asm volatile ("lfence; rdtsc; lfence" : "=a"(lo), "=d"(hi));
    return ((qword)hi << 32) | (qword)lo;
}

#endif
