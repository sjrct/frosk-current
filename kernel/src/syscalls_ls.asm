;
; kernel/src/syscalls_ls.asm
;

[section .rodata]

%macro FUNC 1
	extern %1
	dq %1
%endmacro

syscalls_ls:
	FUNC pub_fs_aquire
	FUNC pub_fs_release
	FUNC pub_fs_first
	FUNC pub_fs_next
	FUNC pub_fs_mkdir
	FUNC pub_fs_mkvdir
	FUNC pub_fs_size
	FUNC pub_fs_read
	FUNC pub_fs_write
	dq 0

syscalls_count: equ ($ - syscalls_ls) / 8

