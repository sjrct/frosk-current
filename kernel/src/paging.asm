;
; paging.asm
;

[bits 64]
[default rel]

%include "stack.inc"
%include "common.inc"

extern page_fault

[section .text]

global setup_paging
setup_paging:
	extern reg_int
	mov rdi, 0xe
	mov rsi, page_fault_int
	call reg_int
	ret


page_fault_int:
	xchg rdi, [rsp]
	PUSH_CALLER
	pushfq

	mov rax, cr2
	mov rsi, rax
	call page_fault

	popfq
	POP_CALLER
	pop rdi
	iretq
