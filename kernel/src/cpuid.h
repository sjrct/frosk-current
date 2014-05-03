//
// kernel/src/cpuid.h
//

#ifndef CPUID_H
#define CPUID_H

#include <types.h>

int has_cpuid(void);
dword cpuid_dword(dword);
void cpuid_string(dword, char *);

#endif
