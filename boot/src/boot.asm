;
; boot/src/boot.asm
;

%include "common.inc"
%include "leftovers.inc"

[bits 16]
[org 0x7C00]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; First Stage Bootloader ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	jmp 0:boot_start
boot_start:
	; Setup segments and stack
	xor ax, ax
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov sp, 0x7C00 ; temporary

	; save regs from bios
	mov [leftovers.partition_ds], ds
	mov [leftovers.partition_si], si
	mov [leftovers.drive_letter], dl

	; set temporary video mode
	mov ax, 2
	int 0x10

	; check that drive extensions are present
	mov ah, 0x41
	mov bx, 0x55AA
	int 0x13
	mov word [error_str], erstr_no_disk_ext
	jc error16

	; load additional boot code
	mov word [error_str], erstr_no_load_boot2
	call do_read

	jmp after_break

erstr_no_disk_ext:
	db 'disk extensions not supported (int 13h, ah=41h failed)', 0
erstr_no_load_boot2:
	db '2nd stage bootloader ignition failure', 0

%include "before/read.inc"
%include "before/error16.inc"

; 512-byte break (mark sector as bootable)
times 0x1FE - ($ - $$) db 0
dw 0xAA55

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Second Stage Bootloader ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

load_boot2:

%include "after/a20.inc"
%include "after/detect.inc"
%include "after/fs.inc"
%include "after/graphics.inc"

after_break:
	; get boot medium information
	mov ah, 0x48
	xor esi, esi
	mov si, leftovers.disk_data
	mov dword [si], 0x42
	int 0x13
	mov word [error_str], erstr_drive_check
	jc error16

	enable_a20
	detect_memory
	load_fs
	load_kernel
	init_graphics

%ifdef __ARCH_X86_64
	; check for long mode presence
	mov eax, 0x80000001
	cpuid
	test edx, 0x20000000
	mov word [error_str], erstr_no_long_mode
	jz error16
%endif

	; switch to protected mode
	cli
	lgdt [gdt32]

	mov eax, cr0
	or ax, 1
	mov cr0, eax

	jmp KERN_CS:pmode_start

; some error messages
erstr_drive_check:
	db 'Could not  (int 0x13 ah=0x42 failed)', 0
erstr_no_long_mode:
	db 'This CPU does not support long mode', 0

[bits 32]
pmode_start:
	; setup seg registers
	mov ax, KERN_DS
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	; setup paging
	cld
	mov ecx, 0x1000
	mov edi, PM_L4_LOC ; L(3-1) should proceed L4 (common.both)
	xor eax, eax
	rep stosd

%ifdef __ARCH_X86_64
	mov dword [PM_L4_LOC], PM_L3_LOC | 7
	mov dword [PM_L3_LOC], PM_L2_LOC | 7
%endif
	mov dword [PM_L2_LOC], PM_L1_LOC | 7

	xor ecx, ecx
.page_loop:
	mov eax, ecx
	shl eax, 12
	or eax, 3
	mov dword [PM_L1_LOC + ecx * 8], eax

	inc ecx
	cmp ecx, 512
	jne .page_loop

%ifdef __ARCH_X86_64
	; switch to long mode
	lgdt [gdt64]

	mov eax, cr4
	or al, 0x20
	mov cr4, eax

	mov eax, PM_L4_LOC
%else
	mov eax, PM_L2_LOC
%endif
	mov cr3, eax
%ifdef __ARCH_X86_64
	mov ecx, 0xC0000080
	rdmsr
	or eax, 0x100
	wrmsr
%endif

	mov eax, cr0
	or eax, 0x80010000
	mov cr0, eax

%ifdef __ARCH_X86_64
	jmp KERN_CS:lmode_start

[bits 64]
lmode_start:
	mov rsp, DEF_STACK_LOC
%endif

	; for both 32 and 64 bit mode
	jmp KERNEL_LOC

%include "after/gdt.inc"

times 0x1000 - ($ - $$) db 0
