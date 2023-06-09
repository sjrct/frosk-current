/*
 * kernel/src/fault.c
 */

#include "debug.h"
#include "fault.h"
#include "interrupt.h"
#include "util.h"

#define DBZ 0   // Divide by zero
#define UOP 6   // Undefined opcode
#define DBL 8   // Double fault
#define SSF 12  // Stack segment fault
#define GPF 13  // General protection fault
#define MAX_FAULT 0x1F

void setup_faults(void)
{
	/* These faults handlers are declared in the assembly */
	void dbz(void), uop(void), dbl(void), ssf(void), gpf(void);

	reg_int(DBZ, dbz);
	reg_int(UOP, uop);
	reg_int(DBL, dbl);
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

void uop_abort(const char * str, ulong addr)
{
	dprintf("Undefined opcode at %p\n", addr);
	dprintf("Hex dump:\n%x %x %x %x\n", atb(addr), atb(addr+1), atb(addr+2), atb(addr+3));
	dprintf("%x %x %x %x\n", atb(addr+4), atb(addr+5), atb(addr+6), atb(addr+7));

	asm volatile ("cli");
	for (;;) {
		asm volatile ("hlt");
	}
}
