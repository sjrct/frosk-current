//
// kernel/src/interrupt.h
//

#ifndef INTERRUPT_H
#define INTERRUPT_H

enum {
	IRQ_TIMER = 0,
	IRQ_PS2_KEYBOARD,
	IRQ_CASCASE,
	IRQ_COM2,
	IRQ_COM1,
	IRQ_LPT2,
	IRQ_FLOPPY,
	IRQ_LPT1,
	
	IRQ_CMOS,
	IRQ_LEGACY1,
	IRQ_LEGACY2,
	IRQ_LEGACY3,
	IRQ_PS2_MOUSE,
	IRQ_FPU,
	IRQ_ATA1,
	IRQ_ATA2,
};

void setup_ints(void);

void xreg_int(int, void (*)(), byte, byte);
void reg_int(int, void (*)());
void reg_irq(int, void (*)());
void set_irq(int);
void clr_irq(int);

void dummy_int(void);

#endif
