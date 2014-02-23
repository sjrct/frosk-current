;
; boot/src/leftovers.inc
;

; set locations for leftovers
; changes to this should be reflected in 'kernel/src/leftovers.h'
leftovers:
	.partition_ds:  equ LEFTOVERS_LOC
	.partition_si:  equ LEFTOVERS_LOC + 2
	.drive_letter:  equ LEFTOVERS_LOC + 4
	.mm_entry_size: equ LEFTOVERS_LOC + 5
	.mm_size:       equ LEFTOVERS_LOC + 6
	.kernel_size:   equ LEFTOVERS_LOC + 8
	.disk_data:     equ LEFTOVERS_LOC + 0xA
	.memory_map:    equ LEFTOVERS_LOC + 0x4C


