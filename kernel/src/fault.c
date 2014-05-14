/*
 * kernel/src/fault.c
 */

#include "debug.h"
#include "fault.h"
#include "interrupt.h"

#define DBZ 0
#define GPF 13

void setup_faults(void)
{
	/* These faults handlers are declared in the assembly */
	void dbz(void), gpf(void);

	reg_int(DBZ, dbz);
	reg_int(GPF, gpf);
}

void abort(const char * str, ulong num)
{
	dprintf(str, num);

	asm volatile ("cli");
	for (;;) {
		asm volatile ("hlt");
	}
}
