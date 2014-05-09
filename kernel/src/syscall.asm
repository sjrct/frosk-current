;
; kernel/src/syscall.asm
;

%include "stack.inc"
%include "common.inc"
%include "syscalls_ls.asm"

extern xreg_int
global setup_syscalls

[default rel]
[section .text]

setup_syscalls:
	mov rdi, SYSCALL_INT
	mov rsi, syscall
	mov rdx, 0xEE
	xor rcx, rcx
	call xreg_int
	ret

syscall:
	cmp rax, syscalls_count
	jae .return

	mov r11, rax
	SAVE_SEGMENTS
	mov rax, r11

	lea r11, [syscalls_ls]
	mov rax, [r11 + rax * 8]
	call rax

	mov r11, rax
	RESTORE_SEGMENTS
	mov rax, r11
.return:
	iretq

[section .rodata]

fmt:
	db 'syscall %d', 10, 0
