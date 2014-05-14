;
; kernel/src/stack.inc
;

%include "common.inc"

%ifdef __ARCH_X86_64
	%macro PUSH_CALLEE 0
		push rbx
		push rbp
		push r12
		push r13
		push r14
		push r15
	%endmacro

	%macro POP_CALLEE 0
		pop r15
		pop r14
		pop r13
		pop r12
		pop rbp
		pop rbx
	%endmacro

	%macro PUSH_CALLER 0
		push rax
		push rcx
		push rdx
		push rsi
		push rdi
		push r8
		push r9
		push r10
		push r11
	%endmacro

	%macro POP_CALLER 0
		pop r11
		pop r10
		pop r9
		pop r8
		pop rdi
		pop rsi
		pop rdx
		pop rcx
		pop rax
	%endmacro
%else
	%macro PUSH_CALLEE 0
		push ebx
		push esi
		push edi
		push ebp
	%endmacro

	%macro POP_CALLEE 0
		pop ebp
		pop edi
		pop esi
		pop ebx
	%endmacro

	%macro PUSH_CALLER 0
		push eax
		push ecx
		push edx
	%endmacro

	%macro POP_CALLER 0
		pop edx
		pop ecx
		pop eax
	%endmacro
%endif

%macro SAVE_SEGMENTS 0
	mov ax, ds
	push ax

	mov ax, es
	push ax

	mov ax, fs
	push ax

	mov ax, gs
	push ax

	mov ax, KERN_DS
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
%endmacro

%macro RESTORE_SEGMENTS 0
	pop ax
	mov gs, ax

	pop ax
	mov fs, ax

	pop ax
	mov es, ax

	pop ax
	mov ds, ax
%endmacro
