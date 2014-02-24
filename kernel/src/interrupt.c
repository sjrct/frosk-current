//
// kernel/src/interrupt.c
//

#include <common.h>
#include "asm.h"
#include "interrupt.h"

#pragma pack(push, 1)
typedef struct idt_entry {
	word  lowaddr;
	word  selector;
	byte  ist;
	byte  flags;
	word  midaddr;
	dword highaddr;
	dword zero;
} idt_entry_t;

static idt_entry_t idt[0x100];

const struct {
	word size;
	qword entries;
} idt_ptr = {
	sizeof idt,
	(qword)idt
};
#pragma pack(pop)

void setup_ints(void)
{
	void setup_irqs(void);

	setup_irqs();
	reg_irq(IRQ_LPT1, dummy_int);

	cli();
	asm volatile ("lidt idt_ptr(%rip)");
}

void xreg_int(int i, void (*func)(), byte fl, byte ist)
{
	qword addr = (qword)func;
	idt[i].lowaddr  = addr & 0xffff;
	idt[i].midaddr  = (addr >> 16) & 0xffff;
	idt[i].highaddr = addr >> 32;
	idt[i].selector = KERN_CS;
	idt[i].flags    = fl;
	idt[i].ist      = ist;
	idt[i].zero     = 0;
}

void reg_int(int i, void (*func)())
{
	xreg_int(i, func, 0x8E, 0);
}

void reg_irq(int i, void (*func)())
{
	reg_int(IRQ0 + i, func);
	set_irq(i);
}
