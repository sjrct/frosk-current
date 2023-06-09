;
; kernel/src/cpuid.asm
;

[default rel]
[section .text]

global has_cpuid
global cpuid_dword
global cpuid_string;



%ifdef __ARCH_X86_64
has_cpuid:
    pushfq
    pop rax
    mov rcx, rax
    xor rcx, 0x200000
    push rcx
    popfq
    pushfq
    pop rcx
    xor rax, rcx
    ret

global get_cpuid
get_cpuid:
    mov eax, edi
    cpuid
    ; cpuid_result_t
    mov [rsi + 0], eax
    mov [rsi + 4], ebx
    mov [rsi + 8], ecx
    mov [rsi + 12], edx
    ret

global cpuid_string
cpuid_string:
    push rbx

    mov rax, rdi
    cpuid
    mov [rsi + 0], rbx
    mov [rsi + 4], rdx
    mov [rsi + 8], rcx

    pop rbx
    ret

%else

    ; TODO
has_cpuid:
cpuid_string:
cpuid_dword:
    xor eax, eax
    ret
%endif
