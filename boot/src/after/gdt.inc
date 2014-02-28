;
; boot/src/gdt.inc
;

; gdt for 32-bit mode
gdt32:
	dw .end - .null_seg
	dd .null_seg
.null_seg:
	dw 0x0000           ; low-word of length
	dw 0x0              ; low-word of base address
	db 0x0              ; mid-byte of base address
	db 0x00             ; flags
	db 0x0              ; access/length
	db 0x0              ; high byte of base address
.code_seg:
	dw 0xffff
	dw 0x0
	db 0x0
	db 0x9a
	db 0xcf
	db 0x0
.data_seg:
	dw 0xffff
	dw 0x0
	db 0x0
	db 0x92
	db 0xcf
	db 0x0
.end:

; gdt for 64-bit mode
%ifidn __ARCH,x86_64
gdt64:
	dw .end - .null_seg
	dd .null_seg
.null_seg:
	dq 0x0000000000000000
.code_seg:
	dq 0x0020980000000000
.data_seg:
	dq 0x0000920000000000
.ucode_seg:
	dq 0x0020F80000000000
.udata_seg:
	dq 0x0000F20000000000
.tss_seg:
	dw 0x0068
	dw TSS_SEG_LOC & 0xFFFF
	db (TSS_SEG_LOC >> 16) & 0xFF
	db 0x89
	db 0x10
	db (TSS_SEG_LOC >> 24) & 0xFF
	dd (TSS_SEG_LOC >> 32) & 0xfFFFfFFF
	dd 0
.end:
%endif
