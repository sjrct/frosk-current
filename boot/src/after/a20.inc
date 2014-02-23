;
; boot/src/after/a20.inc
;

%macro enable_a20 0
	; TODO check if a20 is enabled/force disable a20?

	; enable a20 line
	call a20_wait_2
	mov al, 0xAD
	out 0x64, al

	call a20_wait_2
	mov al, 0xD0
	out 0x64, al

	call a20_wait_1
	in al, 0x60
	mov ah, al

	call a20_wait_2
	mov al, 0xD1
	out 0x64, al

	call a20_wait_2
	mov al, ah
	or al, 2
	out 0x60, al

	call a20_wait_2
	mov al, 0xAE
	out 0x64, al
%endmacro

; wait functions for enabling the a20 line
a20_wait_1:
	in al, 0x64
	test al, 1
	jz a20_wait_2
	ret

a20_wait_2:
	in al, 0x64
	test al, 2
	jnz a20_wait_2
	ret

