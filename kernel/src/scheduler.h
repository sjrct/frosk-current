//
// kernel/src/scheduler.h
//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <types.h>

typedef struct thread thread_t;
typedef struct process process_t;

struct thread {
	thread_t * next_inproc;
	thread_t * next_sched;
	process_t * parent;
	uint priority;
};

struct process {
	thread * first_thread;
};

extern thread_t * curthrd;

process_t * spawn(void);
thread_t * branch(process_t *, qword);

#endif
