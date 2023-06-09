[bits 64]
[default rel]
[section .text]

; 0 = eax -> cpuid
; 1 = register to check (eax, ebx, ecx, edx)
; 2 = bit to check (0-31)
%macro CHECK_CPUID 3
    push rbx

    mov eax, %1
    cpuid
    mov rax, 1 << %3
    and eax, %2

    pop rbx
    ret
%endmacro

global sse_available
sse_available:
    CHECK_CPUID 1, edx, 25

global sse2_available
sse2_available:
    CHECK_CPUID 1, edx, 26

global sse3_available
sse3_available:
    CHECK_CPUID 1, ecx, 0

global fxsr_available
fxsr_available:
    CHECK_CPUID 1, edx, 24

global mmx_available
mmx_available:
    CHECK_CPUID 1, edx, 23

global enable_sse
enable_sse:
    mov rax, cr0
    and ax, ~4       ; clear CR0.EM
    or  ax, 2        ; set CR0.MP
    mov cr0, rax
    mov rax, cr4
    or  ax, 3 << 9   ; OSFXSR & OSXMMEXCPT
    mov cr4, rax
    ret
