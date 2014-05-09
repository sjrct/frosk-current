;
; boot/src/after/graphics.inc
;

; get graphics mode info and sets graphics mode
%macro init_graphics 0
;	mov ax, 0x4f01
;	mov cx, GRAPHICS_MODE
;	mov di, leftovers.vm_info
;	int 0x10
;	cmp ax, 0x4f
;	mov word [error_str], erstr_get_grinfo
;	jne error16

	mov ax, 0x4f02
	mov bx, GRAPHICS_MODE
	int 0x10
	cmp ax, 0x4f
	mov word [error_str], erstr_set_grmode
	jne error16
%endmacro

; error strings
erstr_get_grinfo:
	db 'Failed to get graphics mode information.', 0
erstr_set_grmode:
	db 'Failed to set graphics mode.', 0

