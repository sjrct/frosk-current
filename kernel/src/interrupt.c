//
// kernel/src/interrupt.c
//

#include <common.h>
#include "asm.h"
#include "interrupt.h"

typedef struct idt_entry {
    word  lowaddr;
    word  selector;
    byte  ist;
    byte  flags;
    word  midaddr;
#ifdef __ARCH_X86_64
    dword highaddr;
    dword zero;
#endif
} __attribute__((packed)) idt_entry_t;

static idt_entry_t idt[0x100];

const struct {
    word size;
    ulong entries;
} __attribute__((packed)) idt_ptr = {
    sizeof idt,
    (ulong)idt
};

void setup_ints(void)
{
    void setup_irqs(void);

    setup_irqs();
    reg_irq(IRQ_LPT1, dummy_int);

    cli();

#ifdef __ARCH_X86_64
    asm volatile ("lidt idt_ptr(%rip)");
#else
    asm volatile ("lidt idt_ptr");
#endif
}

void xreg_int(int i, void (*func)(), byte fl, byte ist)
{
    ulong addr = (ulong)func;
    idt[i].lowaddr  = addr & 0xffff;
    idt[i].midaddr  = (addr >> 16) & 0xffff;
    idt[i].selector = KERN_CS;
    idt[i].flags    = fl;
    idt[i].ist      = ist;
#ifdef __ARCH_X86_64
    idt[i].highaddr = addr >> 32;
    idt[i].zero     = 0;
#endif
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

int is_interrupt_set(int i)
{
#ifdef __ARCH_X86_64
    return idt[i].lowaddr != 0 || idt[i].midaddr != 0 ||  idt[i].highaddr != 0;
#else
    return idt[i].lowaddr != 0 || idt[i].midaddr != 0;
#endif
}
