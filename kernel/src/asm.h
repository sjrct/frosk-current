//
// kernel/src/asm.h
//

#ifndef ASM_H
#define ASM_H

#include <types.h>

#ifndef INLINE
	#define INLINE __attribute__((always_inline)) static inline
#endif

#define _ASM_IO(T, M) \
INLINE void out##M(ushort port, T data) { \
	asm volatile ("out" #M " %0, %1" : : "a" (data), "Nd" (port)); \
} \
INLINE T in##M(ushort port) { \
	T got; \
	asm volatile ("in" #M " %1, %0" : "=a" (got) : "Nd" (port)); \
	return got; \
}

#define _ASM_0(S) \
INLINE void S(void) { \
	asm volatile( #S ); \
}

#define _ASM_REG(R) \
INLINE ulong get##R(void) { \
	ulong r; \
	asm volatile ("movq %%" #R ", %0" : "=r" (r)); \
	return r; \
}

_ASM_IO(byte,  b)
_ASM_IO(word,  w)
_ASM_IO(dword, l)

_ASM_0(hlt)
_ASM_0(pause)
_ASM_0(cli)
_ASM_0(sti)

_ASM_REG(rax)
_ASM_REG(rbx)
_ASM_REG(rcx)
_ASM_REG(rdx)
_ASM_REG(rdi)
_ASM_REG(rsi)
_ASM_REG(rsp)
_ASM_REG(rbp)
_ASM_REG(r8)
_ASM_REG(r9)
_ASM_REG(r10)
_ASM_REG(r11)
_ASM_REG(r12)
_ASM_REG(r13)
_ASM_REG(r14)
_ASM_REG(r15)

INLINE void io_wait(void) {
	asm volatile("outb %%al, $0x80" :: "a" (0));
}

INLINE ulong getfl(void) {
	ulong r;
	asm volatile ("pushfq; movq (%%rsp), %0; addq $8, %%rsp" : "=r" (r));
	return r;
}

INLINE void invlpg(ulong pg) {
	asm volatile ("invlpg (%0)" :: "r" (pg) : "memory");
}

#undef _ASM_IO
#undef _ASM_0
#undef _ASM_REG

#endif
