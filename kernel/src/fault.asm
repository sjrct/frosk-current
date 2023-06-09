;
; kernel/src/fault.asm
;

[default rel]
[section .text]

extern abort
extern uop_abort

global dbz
global uop
global dbl
global ssf
global gpf

%ifdef __ARCH_X86_64

dbz:
	push rdi
	push rsi

	xor rsi, rsi
	mov rdi, dbz_str
	call abort

	pop rsi
	pop rdi
	iretq

uop:
	push rdi
	push rsi

	mov rsi, [rsp + 0x10]
	mov rdi, uop_str
	call uop_abort

	pop rsi
	pop rdi
	iretq

dbl:
	cli
	;mov rsi, [rsp + 0x10]
	;mov rdi, dbl_str
	;call abort
.hlt:
	hlt
	jmp .hlt

gpf:
	push rdi
	push rsi

	mov rsi, [rsp + 16]
	mov rdi, gpf_str
	call abort

	pop rsi
	pop rdi
	add esp, 8
	iretq

ssf:
	push rdi
	push rsi

	mov rsi, [rsp + 16]
	mov rdi, ssf_str
	call abort

	pop rsi
	pop rdi
	add esp, 8
	iretq

%else

dbz:
	push dword 0
	push dbz_str
	call abort
	add esp, 8
	iret

uop:
	push dword 0
	push uop_str
	add esp, 8
	iret

gpf:
	push gpf_str
	call abort
	add esp, 8
	iret

ssf:
	push ssf_str
	call abort
	add esp, 8
    iret

%endif

[section .rodata]

dbz_str: db `Division by zero.\n`, 0
uop_str: db `Undefined opcode at %X.\n`, 0
dbl_str: db `Double fault at maybe %X.\n`, 0
ssf_str: db `Stack segment fault: %X\n`, 0
gpf_str: db `General protection fault: %X\n`, 0
