/*
 * include/kernel/util.h
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <types.h>
#include <stddef.h>

#define at(ADDR, TYPE) (* (TYPE *) ((ulong)(ADDR)))
#define atb(X) at((X), byte)
#define atw(X) at((X), word)
#define atd(X) at((X), dword)
#define atq(X) at((X), qword)

#define align(X, TO) ((X) % (TO) ? (X) - ((X) % (TO)) + (TO) : (X))
#define ceil(X, TO) align(X, TO)
#define floor(X, TO) ((X) % (TO) ? (X) - ((X) % (TO)) : (X))

#define access(FROM,FLD,DEF) ((FROM) == NULL ? (DEF) : (FROM)->FLD)

#define lb(X) (bat(X, 0))
#define hb(X) (bat(X, 1))
#define bat(X,I) (((X) >> (I)*8) & 0xff)

#endif
