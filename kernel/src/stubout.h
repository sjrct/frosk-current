//
// kernel/src/stubout.h
//

#ifndef STUBOUT_H
#define STUBOUT_H

#include <stdarg.h>

void dputc(char);
void dputu(unsigned long);
void dputl(long);
void vdprintf(const char *, va_list);
void dprintf(const char *, ...);

#endif
