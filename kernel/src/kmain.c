/*
 * kernel/src/kmain.c
 */

#include "exec.h"
#include "cpuid.h"
#include "debug.h"
#include "fault.h"
#include "common.h"
#include "paging.h"
#include "stubout.h"
#include "syscall.h"
#include "leftovers.h"
#include "interrupt.h"
#include "fs/fs.h"
#include "dev/dev.h"
#include "memory/pages.h"
#include "memory/kernel.h"

static void init_bss(void);
static void init_ro(void);

void __attribute__((noreturn)) kmain(void)
{

	init_bss();
	init_ro();

	setup_kernel_memory();
	setup_pages();
	setup_ints();
	setup_faults();
	setup_fs();
	setup_syscalls();

	init_devs();


	char vendor[12];
	if (has_cpuid()) {
		cpuid_string(0, vendor);
		dprintf("CPU Vendor ID: %s\n");
	}

	dprintf("start = %p\n", fexec("/prgm/start", 0, NULL, NULL));
	start_scheduler();

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

	assert(!( (ulong)_ro_start & (PAGE_SIZE-1) ));

	for (; x < _ro_end; x += PAGE_SIZE) {
		pageto((ulong)x, (ulong)x | 1);
	}
}

