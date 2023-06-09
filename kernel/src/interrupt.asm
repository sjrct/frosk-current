;
; interrupt.asm
;

[default rel]

%include "common.inc"

%define PIC1 0x20
%define PIC2 0xA0

%define io_wait out 0x80, al

[section .text]

global setup_irqs
setup_irqs:
	mov al, 0x11
	out PIC1, al
	io_wait
	out PIC2, al
	io_wait

	mov al, IRQ0
	out PIC1 + 1, al
	io_wait
	add al, 8
	out PIC2 + 1, al
	io_wait

	mov al, 4
	out PIC1 + 1, al
	io_wait
	mov al, 2
	out PIC2 + 1, al
	io_wait

	mov al, 5
	out PIC1 + 1, al
	io_wait
	mov al, 1
	out PIC2 + 1, al
	io_wait

	mov al, 0xff
	out PIC1 + 1, al
	out PIC2 + 1, al

	ret


global set_irq
set_irq:
	cmp di, 7
	ja .servant
	mov dx, PIC1 + 1
	jmp .master
.servant:
	mov dx, PIC2 + 1
	sub di, 8
.master:

	mov ecx, edi
	mov si, 1
	shl esi, cl
	not esi

	in al, dx
	and eax, esi
	out dx, al

	ret


global clr_irq
clr_irq:
	cmp di, 7
	ja .servant
	mov dx, PIC1 + 1
	jmp .master
.servant:
	mov dx, PIC2 + 1
	sub di, 8
.master:

	mov ecx, edi
	mov si, 1
	shl esi, cl

	in al, dx
	or eax, esi
	out dx, al

	ret


global dummy_int
dummy_int:
%ifdef __ARCH_X86_64
	iretq
%else
	iret
%endif
