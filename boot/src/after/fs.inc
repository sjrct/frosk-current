;
; boot/src/after/fs.inc
;

; loads and checks the validity of the first fs meta block
%macro load_fs 0
	; read fs block
	mov word  [dap + 2], 8
	mov dword [dap + 4], RFSMB_LOC
	mov dword [dap + 8], 8
	call do_read ; 4

	; check file system mark
	mov word [error_str], erstr_no_filesystem
	mov di, RFSMB_LOC
	mov si, fs_mark

	mov eax, [si]
	cmp eax, [di]
	jne error16

	mov eax, [si + 4]
	cmp eax, [di + 4]
	jne error16

;	cld
;	mov cx, 1
;	mov si, fs_mark
;	mov di, RFSMB_LOC
;	repe cmpsb
;	jne error16
%endmacro

; locates and loads the kernel
%macro load_kernel 0
_load_kernel:
	; locate the kernel (warning: some lba/size data is lost)
	mov esi, [RFSMB_LOC + 0x10]
	call deref
	mov esi, [si + 0x10]

.search_loop:
	call deref

	mov bx, si
	mov esi, [si + 4]
	call deref

	add si, 0x10
	mov di, kernel_name
	mov ax, kernel_name.size

.cmp_loop:
	dec ax
	mov dl, [si]
	cmp dl, [di]
	jne .continue
	inc si
	inc di
	test ax, ax
	jnz .cmp_loop
	jmp .found
.continue:

	mov esi, [bx + 0xC]
	test esi, esi
	jnz .search_loop

	mov word [error_str], erstr_no_kernel
	jmp error16
.found:

	mov esi, [bx + 0x10]
	call deref
	mov eax, [si + 0x10]
	mov cx, ax
 	shr eax, 9
 	and cx, 0x1ff
 	test cx, cx
 	jz .skip
 	inc eax
.skip:
	mov word [dap + 2], ax
	mov word [leftovers.kernel_size], ax

	mov eax, [si + 8]
	mov ecx, eax
	shl eax, 3
	shr ecx, 29
	mov [dap + 8], eax
	shl eax, 3
	or eax, ecx
	mov eax, [si + 12]
	mov [dap + 12], eax

	mov dword [dap + 4], KERNEL_LOC % 0x10000
	mov word [dap + 6], (KERNEL_LOC / 0x10000) * 0x1000

	mov word [error_str], erstr_cant_read_kernel
	call do_read
%endmacro

; dereference a file system pointer
deref:
	shl si, 4
	add si, RFSMB_LOC
	ret

; error strings
erstr_no_filesystem:
	db "There doesn't seem to be a valid filesystem here.", 0
erstr_no_kernel:
	db "I can't find the kernel, did you misplace it?", 0
erstr_cant_read_kernel:
	db "I can't read the kernel. I tried 3 times, then I gave up.", 0

; file system mark
fs_mark:
	db 'Frosk FS'

; kernel name
kernel_name:
	db 'kernel.bin'
.size: equ $ - kernel_name

