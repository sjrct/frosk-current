//
// include/kernel/stdarg.h
// include/prgm/stdarg.h
// include/lib/stdarg.h
//

#ifndef _STDARG_H_
#define _STDARG_H_

typedef __builtin_va_list va_list;

#define va_start(X,Y) __builtin_va_start(X,Y)
#define va_end(X)     __builtin_va_end(X)
#define va_arg(X,Y)   __builtin_va_arg(X,Y)

#endif
