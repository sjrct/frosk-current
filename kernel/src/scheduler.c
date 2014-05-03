//
// kernel/src/scheduler.c
//

#include "scheduler.h"
#include "memory/kernel.h"

thread_t * curthread = NULL;

process_t * spawn(void)
{
	process_t * p = kalloc(sizeof(process_t));
	p->first_thread = NULL;
	return p;
}

thread_t * branch(process_t * proc, qword ip)
{
	thread_t * t = kalloc(sizeof(thread_t));

	t->next_inproc = proc->first;
	proc->first = t;

	if (curthrd == NULL) {
		t->next_sched = curthrd = t;
	} else {
		t->next_sched = curthrd->next;
		curthrd->next = t;
	}

	return t;
}
