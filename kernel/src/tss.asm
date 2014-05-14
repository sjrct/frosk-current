;
; kernel/src/tss.asm
;

[bits 64]

%include "common.inc"

global setup_tss

[section .text]

setup_tss:
	xor rax, rax
	mov rdi, TSS_SEG_LOC
	mov rcx, 0x68
	rep stosb

	mov rax, LOW_HALF_TOP - 4
	mov qword [TSS_SEG_LOC + 4], rax
	mov dword [TSS_SEG_LOC + 0x64], 0x68
	mov ax, TSS_SEL | 3
	ltr ax
	ret

