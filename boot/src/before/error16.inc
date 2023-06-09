;
; boot/src/before/error16.inc
;

%define ERROR_INK 4

; writes a character to the screen
;  al = character
;  trashes ax, bx, cx
show_char:
	mov bx, [error_cursor]
	cmp al, 0xa
	jne .not_newline

	mov ax, bx
	mov cx, 160
	div cl
	shr ax, 8
	sub bx, ax
	add bx, cx
	mov [error_cursor], bx
	ret

.not_newline:
	mov [fs:bx], al
	mov byte [fs:bx + 1], ERROR_INK
	add bx, 2
	mov [error_cursor], bx
	ret

; writes a string to the screen
;  di = string pointer
;  trashes ax, bx, cx, di
show_string:
	mov al, [di]
	test al, al
	jz .return

	call show_char
	inc di
	jmp show_string
.return:
	ret

; writes an unisnged hex number to the screen
;  ecx = number
;  trashes ax, bx, ecx, di
show_hex:
	push ecx

	mov di, hex_num_str
	mov al, ' '
	mov cx, 8
	rep stosb

	pop ecx
	dec di

	test ecx, ecx
	jnz .loop
	mov byte [di], '0'
	jmp .return

.loop:
	mov al, cl
	and al, 0xf
	cmp al, 0xa
	jl .digit
	add al, 'a' - 10
	jmp .alpha
.digit:
	add al, '0'
.alpha:
	mov [di], al
	dec di
	shr ecx, 4
	test ecx, ecx
	jnz .loop

.return:
	mov di, hex_num_str
	call show_string
	ret

; shows a register's value
; registers value in ecx
;  trashes ax, bx, di
show_reg:
	mov di, reg_name
	call show_string
	call show_hex
	mov al, 0xa
	call show_char
	inc byte [reg_name + 1]
	ret

; an error occurred in 16-bit mode
error16:
	push edi
	push ecx
	push ebx
	push eax

	; clear screen
	mov ax, 0xb800
	mov fs, ax
	mov es, ax
	mov cx, 80*25*2
	xor di, di
	rep stosb

	xor ax, ax
	mov es, ax

	; preface output
	mov di, boot_error_msg
	call show_string

	mov di, [error_str]
	call show_string

	mov di, reg_dump_preface
	call show_string

	; dump registers
	pop ecx
	call show_reg

	pop ecx
	call show_reg

	pop ecx
	call show_reg

	mov ecx, edx
	call show_reg

	mov byte [reg_name + 1], 'd'
	mov byte [reg_name + 2], 'i'
	pop ecx
	call show_reg

	mov byte [reg_name + 1], 's'
	mov ecx, esi
	call show_reg

	mov byte [reg_name + 1], 's'
	mov byte [reg_name + 2], 'p'
	mov ecx, esp
	call show_reg

	mov byte [reg_name + 1], 'b'
	mov ecx, ebp
	call show_reg

	; halt machine
	cli
.hltlp:
	hlt
	jmp .hltlp

; error data
boot_error_msg:
	db 'Boot error:', 0xa, 0
reg_dump_preface:
	db 0xa, 0xa, 'Registers:', 0xa, 0
reg_name:
	db 'eax = ', 0
hex_num_str:
	db 0,0,0,0, 0,0,0,0, 'h', 0
error_cursor:
	dw 0
error_str:
	dw 0
