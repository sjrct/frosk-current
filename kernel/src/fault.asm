;
; kernel/src/fault.asm
;

[default rel]
[section .text]

extern abort

global dbz
global gpf

%ifdef __ARCH_X86_64

dbz:
	push rdi
	mov rax, dbz_str
	call abort
	pop rdi
	iretq

gpf:
	push rdi
	mov rdi, gpf_str
	call abort
	pop rdi
	iretq

%else

dbz:
	push dbz_str
	call abort
	pop rax
	iret

gpf:
	push gpf_str
	call abort
	pop rax
	iret

%endif

[section .rodata]

dbz_str: db `Division by zero.\n`, 0
gpf_str: db `General protection fault.\n`, 0
