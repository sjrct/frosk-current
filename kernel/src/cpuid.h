//
// kernel/src/cpuid.h
//

#ifndef CPUID_H
#define CPUID_H

#include <types.h>

typedef struct cpuid_result {
    dword eax, ebx, ecx, edx;
} cpuid_result_t;

int has_cpuid(void);
// Fills cpuid_result_t and returns eax
void get_cpuid(dword rax, cpuid_result_t *);
void cpuid_string(dword, char *);

#endif
