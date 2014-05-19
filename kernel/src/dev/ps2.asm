;
; kernel/src/dev/ps2.asm
;

%include "stack.inc"

extern kb_put
global ps2_kb_irq

[default rel]
[section .text]

ps2_kb_irq:
	PUSH_CALLER

	xor rax, rax
	in al, 0x60
	mov rdi, rax
	call kb_put

	mov al, 0x20
	out 0x20, al

	POP_CALLER
	iretq
