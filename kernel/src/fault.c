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

extern struct saved_state {
    qword rax; qword rbx; qword rcx; qword rdx;
    qword rsi; qword rdi; qword rbp; qword rsp;
    qword r8;  qword r9;  qword r10; qword r11;
    qword r12; qword r13; qword r14; qword r15;
} saved_state;

void setup_faults(void)
{
    int i;

    /* These faults handlers are declared in the assembly */
    void dbz(void), uop(void), dbl(void), ssf(void), gpf(void);
    void undefined_fault(void);


    reg_int(DBZ, dbz);
    reg_int(UOP, uop);
    reg_int(DBL, dbl);
    reg_int(SSF, ssf);
    reg_int(GPF, gpf);

    for (i = 0; i <= MAX_FAULT; i++) {
        if (!is_interrupt_set(i)) {
            reg_int(i, undefined_fault);
        }
    }

}

void abort(const char * str, ulong num)
{
    dprintf(str, num);

    dprintf(
        "Registers:\n"
        "  rax = %X\n" "  rbx = %X\n" "  rcx = %X\n" "  rdx = %X\n"
        "  rsi = %X\n" "  rdi = %X\n" "  rbp = %X\n" "  rsp = %X\n"
        "  r8  = %X\n" "  r9  = %X\n" "  r10 = %X\n" "  r11 = %X\n"
        "  r12 = %X\n" "  r13 = %X\n" "  r14 = %X\n" "  r15 = %X\n"
        , saved_state.rax, saved_state.rbx, saved_state.rcx, saved_state.rdx
        , saved_state.rsi, saved_state.rdi, saved_state.rbp, saved_state.rsp
        , saved_state.r8,  saved_state.r9,  saved_state.r10, saved_state.r11
        , saved_state.r12, saved_state.r13, saved_state.r14, saved_state.r15
    );

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
