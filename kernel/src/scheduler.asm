;
; kernel/src/scheduler.asm
;

[default rel]

%include "stack.inc"
%include "common.inc"

extern set_irq
extern swapflop
extern scheduler_lock
extern head_thrd
extern head_proc

extern dprintf

; reflected in include/kernel/scheduler.h
struc process
    .next:      resq 1
    .parent:    resq 1
    .first:     resq 1
    .code:      resq 1
    .argv:      resq 1
    .argc:      resd 1
    .timeslice: resd 1
    ; mqueue here
endstruc

struc thread
    .parent:      resq 1
    .next_inproc: resq 1
    .next_sched:  resq 1
    .stack:       resq 1
    .saved_rsp:   resq 1
    .state:       resd 1
    .page_fl:     resd 1
    .fxsave_block:resb 512
endstruc

[section .text]

; this function handles task switching
global timer_irq
timer_irq:
    push rax
    mov al, 0x20
    out 0x20, al
    pop rax

    call yield
    iretq


global yield
yield:
    pushfq
    PUSH_CALLER
    PUSH_CALLEE
    SAVE_SEGMENTS

    ; save fpu state
    mov rcx, [cur_thrd]
    test rcx, rcx
    jz .ct_null3
    fxsave64 [rcx + thread.fxsave_block]
.ct_null3:

    push qword 0 ; say that context data is pushed

    ; check locking
    test byte [scheduler_lock], 1
    jz .return_early



    ; find ready thread
    mov rbx, [abs head_thrd]
    test rbx, rbx
    jz .return_early
    mov rax, rbx

.search_loop:
    cmp byte [rbx + thread.state], 0
    jne .search_loop_not_done
    cmp rbx, rcx
    jne .search_loop_done
.search_loop_not_done:

    mov rbx, [rbx + thread.next_sched]
    cmp rax, rbx
    je .return_early
    jmp .search_loop
.search_loop_done:

    ; update state/next thread
    mov byte [rbx + thread.state], 2
    mov rax, [rbx + thread.next_sched]
    mov [head_thrd], rax

    ; swap in code pages
    xor rdi, rdi
    mov rcx, [cur_thrd]
    test rcx, rcx
    jz .ct_null

    mov rcx, [rcx + thread.parent]
    mov rdi, [rcx + process.code]
.ct_null:

    mov rsi, [rbx + thread.parent]
    mov rsi, [rsi + process.code]
    mov rdx, 0x7
    call swapflop

    ; change old thread status
    xor rdi, rdi
    mov rax, [cur_thrd]
    test rax, rax
    jz .ct_null2

    mov rdi, [rax + thread.stack]
    mov byte [rax + thread.state], 0
    mov [rax + thread.saved_rsp], rsp
.ct_null2:
    mov [cur_thrd], rbx

    ; swap in stack
    mov rsp, 0x70000 ; FIXME there might be a better solution to this
    mov rsi, [rbx + thread.stack]
    mov rdx, 0x7
    call swapflop

    ; load rsp
    mov rsp, [rbx + thread.saved_rsp]

.return_early:

    ; interrupt finished/timer reset
    mov al, 0x1
    out 0x40, al
    out 0x40, al

    pop rax
    test rax, rax
    jnz .do_iretq

    mov rcx, [cur_thrd]
    test rcx, rcx
    jz .ct_null4
    fxrstor64 [rcx + thread.fxsave_block]
.ct_null4:

    RESTORE_SEGMENTS
    POP_CALLEE
    POP_CALLER
.already_locked:
    popfq
    ret
.do_iretq:
    iretq

[section .data]

global preamble_size
global preamble_code
preamble_size: equ preamble_code.end - preamble_code
preamble_code:
    mov ax, USER_DS | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pop rdi
    pop rsi
    pop rax

    call rax

    ; TODO System call to end thread
    mov r15, 0xc0de15dead
    jmp $
.end:

mismatch_str:
    db "! got %X expected %X for offset %X", 10, 0
foobie:
    db "! foobie!", 10, 0

align 8

global cur_thrd
cur_thrd: dq 0
