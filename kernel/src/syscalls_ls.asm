;
; kernel/src/syscalls_ls.asm
;

[section .rodata]

%macro FUNC 1
	extern %1
	dq %1
%endmacro

syscalls_ls:
	dq 0

syscalls_count: equ ($ - syscalls_ls) / 8

