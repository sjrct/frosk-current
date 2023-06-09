;
; boot/src/before/read.inc
;

; reads from disk using dap
do_read:
    mov cx, 4
    mov dl, [leftovers.drive_letter]
.read_loop:
    mov si, dap
    mov ah, 0x42
    int 0x13
    jnc .read_good

    loop .read_loop
    jmp error16
.read_good:
    ret

; data address packet
dap:
    db 0x10       ; packet size
    db 0          ; unused
    dw 7          ; sector count
    dd load_boot2 ; destination
    dq 1          ; lba
