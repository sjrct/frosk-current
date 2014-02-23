//
// kernel/src/kmain.c
//

#include "common.h"
#include "stubout.h"

static void init_bss(void);
static void init_ro(void);

void __attribute__((noreturn)) kmain(void)
{
	init_bss();

	dputs("jello");

	for (;;) {
		asm("hlt");
	}
}

static void init_bss(void)
{
	extern char _bss_start[];
	extern char _bss_end[];

	char * x = _bss_start;

	for (; x < _bss_end; x++) {
		*x = 0;
	}
}

static void init_ro(void)
{
	extern char _ro_start[];
	extern char _ro_end[];

	char * x = _ro_start;

//	assert(!( (ulong)_ro_start & (PAGE_SIZE-1) ));

	for (; x < _ro_end; x += PAGE_SIZE) {
//		pageto((ulong)x, (ulong)x | 1);
	}
}
