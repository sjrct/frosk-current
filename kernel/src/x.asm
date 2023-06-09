%line 1+1 simd.asm
[bits 64]
[default rel]
[section .text]




%line 21+1 simd.asm

[global sse_available]
sse_available:
%line 9+1 simd.asm
 push rcx
 push rdx

 mov eax, 1
 cpuid
 mov rax, 1 << 25
 and rax, edx

 pop rdx
 pop rcx
 ret
%line 25+1 simd.asm

[global sse2_available]
sse2_available:
%line 9+1 simd.asm
 push rcx
 push rdx

 mov eax, 1
 cpuid
 mov rax, 1 << 26
 and rax, edx

 pop rdx
 pop rcx
 ret
%line 29+1 simd.asm

[global sse3_available]
sse3_available:
%line 9+1 simd.asm
 push rcx
 push rdx

 mov eax, 1
 cpuid
 mov rax, 1 << 0
 and rax, ecx

 pop rdx
 pop rcx
 ret
%line 33+1 simd.asm

[global fxsr_available]
fxsr_available:
%line 9+1 simd.asm
 push rcx
 push rdx

 mov eax, 1
 cpuid
 mov rax, 1 << 24
 and rax, edx

 pop rdx
 pop rcx
 ret
%line 37+1 simd.asm

[global mmx_available]
mmx_available:
%line 9+1 simd.asm
 push rcx
 push rdx

 mov eax, 1
 cpuid
 mov rax, 1 << 23
 and rax, edx

 pop rdx
 pop rcx
 ret
