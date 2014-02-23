;
; boot/src/after/detect.inc
;

; detects memory regions on the system
%macro detect_memory 0
_detect_memory:
	mov word [error_str], erstr_detect_memory

	xor ebx, ebx
	mov di, leftovers.memory_map
	mov edx, 0x534D4150 ; !~MAGUS~!
.mm_loop:
	mov ecx, 24
	mov eax, 0xE820
	int 0x15

	jc .mm_done
	cmp eax, 0x534D4150
	jne error16
	test ebx, ebx
	jz .mm_done

	add di, 24
	jmp .mm_loop

.mm_done:
	sub di, leftovers.memory_map - 24
	test di, di
	jz error16
	mov [leftovers.mm_size], di
	mov [leftovers.mm_entry_size], cl
%endmacro

erstr_detect_memory:
	db 'Could not detect memory on system (did you forget the RAM?)', 0

