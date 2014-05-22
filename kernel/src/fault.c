/*
 * kernel/src/fault.c
 */

#include "debug.h"
#include "fault.h"
#include "interrupt.h"

#define DBZ 0
#define UOP 6
#define SSF 12
#define GPF 13

void setup_faults(void)
{
	/* These faults handlers are declared in the assembly */
	void dbz(void), uop(void), ssf(void), gpf(void);

	reg_int(DBZ, dbz);
	reg_int(UOP, uop);
	reg_int(SSF, ssf);
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
