;
; kernel/src/paging.asm
;

[default rel]

%include "stack.inc"
%include "common.inc"

%define PAGE_FAULT_NUM 0xE

extern reg_int
extern page_fault
global setup_paging

[section .text]

%ifdef __ARCH_X86_64

setup_paging:
    mov rdi, PAGE_FAULT_NUM
    mov rsi, page_fault_int
    call reg_int
    ret

page_fault_int:
    cli
    xchg rdi, [rsp]
    push rbp
    mov rbp, rsp
    PUSH_CALLER

    mov rdx, [rbp + 16]
    mov rax, cr2
    mov rsi, rax
    call page_fault

    POP_CALLER
    pop rbp
    pop rdi
    iretq

%else

setup_paging:
    push dword page_fault_int
    push dword PAGE_FAULT_NUM
    call reg_int
    add esp, 8
    ret

page_fault_int:
    PUSH_CALLER

    mov eax, cr2
    push eax
    call page_fault
    pop eax

    POP_CALLER
    iret

%endif
