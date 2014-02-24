//
// kernel/src/stubout.h
//

#ifndef STUBOUT_H
#define STUBOUT_H

#include <types.h>
#include <stdarg.h>

void dputc(char);
void dputs(const char *);
void dputu(ulong, int);
void dputl(long, int);
void vdprintf(const char *, va_list);
void dprintf(const char *, ...);

#endif
