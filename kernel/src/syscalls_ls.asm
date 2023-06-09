;
; kernel/src/syscalls_ls.asm
;

[section .rodata]

; FIXME x86

%macro FUNC 1
	extern %1
	dq %1
%endmacro

syscalls_ls:
    FUNC pub_bexec
    FUNC pub_fexec
	FUNC pub_fs_acquire
	FUNC pub_fs_release
	FUNC pub_fs_first
	FUNC pub_fs_next
	FUNC pub_fs_mkdir
	FUNC pub_fs_mkvdir
	FUNC pub_fs_size
	FUNC pub_fs_read
	FUNC pub_fs_write
	FUNC proc_send_msg
	FUNC proc_recv_msg
	FUNC proc_consume_msg
	FUNC proc_stop_recv
	dq 0

syscalls_count: equ ($ - syscalls_ls) / 8

