;
; kernel/src/lock.asm
;

[default rel]
[section .text]

global trylock
global getlock
global unlock

%ifdef __ARCH_X86_64

trylock:
	xor rax, rax
	lock xchg [rdi], al
	ret

unlock:
	mov byte [rdi], 1
	ret

%else ; i386

trylock:
	mov ecx, [esp + 4]
	xor eax, eax
	lock xchg [ecx], al
	ret

unlock:
	mov ecx, [esp + 4]
	mov byte [ecx], 1
	ret

%endif

; This should work in both i386 and x86-64
getlock:
	call trylock
	test al, al
	jz getlock
	ret
