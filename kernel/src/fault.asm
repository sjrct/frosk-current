;
; kernel/src/fault.asm
;

[default rel]
[section .text]

extern abort
extern uop_abort

global dbz
global uop
global dbl
global ssf
global gpf
global undefined_fault

%ifdef __ARCH_X86_64

%macro SAVE_STATE 0
    mov [saved_state.rax], rax
    mov [saved_state.rbx], rbx
    mov [saved_state.rcx], rcx
    mov [saved_state.rdx], rdx
    mov [saved_state.rsi], rsi
    mov [saved_state.rdi], rdi
    mov [saved_state.rsp], rsp
    mov [saved_state.rbp], rbp
    mov [saved_state.r8],  r8
    mov [saved_state.r9],  r9
    mov [saved_state.r10], r10
    mov [saved_state.r11], r11
    mov [saved_state.r12], r12
    mov [saved_state.r13], r13
    mov [saved_state.r14], r14
    mov [saved_state.r15], r15
%endmacro

dbz:
    SAVE_STATE
    push rdi
    push rsi

    xor rsi, rsi
    mov rdi, dbz_str
    call abort

    pop rsi
    pop rdi
    iretq

uop:
    SAVE_STATE
    push rdi
    push rsi

    mov rsi, [rsp + 0x10]
    mov rdi, uop_str
    call uop_abort

    pop rsi
    pop rdi
    iretq

dbl:
    SAVE_STATE
    cli
    ;mov rsi, [rsp + 0x10]
    ;mov rdi, dbl_str
    ;call abort
.hlt:
    hlt
    jmp .hlt

gpf:
    SAVE_STATE
    push rdi
    push rsi

    mov rsi, [rsp + 16]
    mov rdi, gpf_str
    call abort

    pop rsi
    pop rdi
    add esp, 8
    iretq

ssf:
    SAVE_STATE
    push rdi
    push rsi

    mov rsi, [rsp + 16]
    mov rdi, ssf_str
    call abort

    pop rsi
    pop rdi
    add esp, 8
    iretq

undefined_fault:
    SAVE_STATE
    push rdi
    push rsi

    mov rsi, [rsp + 16]
    mov rdi, undefined_fault_str
    call abort

    pop rsi
    pop rdi
    add esp, 8
    iretq

%else

dbz:
    push dword 0
    push dbz_str
    call abort
    add esp, 8
    iret

uop:
    push dword 0
    push uop_str
    add esp, 8
    iret

gpf:
    push gpf_str
    call abort
    add esp, 8
    iret

ssf:
    push ssf_str
    call abort
    add esp, 8
    iret

%endif

[section .rodata]

dbz_str: db `Division by zero.\n`, 0
uop_str: db `Undefined opcode at %X.\n`, 0
dbl_str: db `Double fault at maybe %X.\n`, 0
ssf_str: db `Stack segment fault: %X\n`, 0
gpf_str: db `General protection fault: %X\n`, 0
undefined_fault_str: db `Undefined fault: %X\n`, 0

[section .data]

global saved_state
saved_state:
.rax: dq 1
.rbx: dq 1
.rcx: dq 1
.rdx: dq 1
.rsi: dq 1
.rdi: dq 1
.rbp: dq 1
.rsp: dq 1
.r8:  dq 1
.r9:  dq 1
.r10: dq 1
.r11: dq 1
.r12: dq 1
.r13: dq 1
.r14: dq 1
.r15: dq 1
