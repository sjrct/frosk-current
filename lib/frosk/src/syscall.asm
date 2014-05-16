;
; lib/frosk/src/syscall.asm
;

[default rel]
[section .text]

%macro SYSCALL 2
global __syscall%1
__syscall%1:
	mov rax, %2
	int 0x40
	ret
%endmacro

SYSCALL 0, rdi
SYSCALL 1, rsi
SYSCALL 2, rdx
SYSCALL 3, rcx
SYSCALL 4, r8
