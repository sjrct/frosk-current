/*
 * include/kernel/scheduler.h
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "memory/region.h"

typedef struct process process_t;
typedef struct thread thread_t;

enum {
	STATE_INVALID = -1,
	STATE_READY = 0,
	STATE_SETUP,
	STATE_RUNNING,
	STATE_WAITING,
	STATE_STOPPED,
};

#pragma pack(push, 1)
/* reflected in kernel/src/scheduler.asm */
struct process {
	process_t * next;
	thread_t * first;
	region_t * code;
	char ** argv;
	int argc;
	dword timeslice;
};

struct thread {
	process_t * parent;
	thread_t * next_inproc;
	thread_t * next_sched;
	region_t * stack;
	qword rsp;
	dword state;
	dword page_fl;
};
#pragma pack(pop)

void start_scheduler(void);
process_t * uspawn(const byte *, ulong, ulong, ulong, int, const char **);
process_t * kspawn(void (*)(void));
thread_t * schedule(process_t *, ulong, int, const qword *, int);
void yield(void);

extern process_t * head_proc;
extern thread_t * head_thrd;
extern thread_t * cur_thrd;

#define cur_proc (access(cur_thrd, parent, NULL))

#endif
