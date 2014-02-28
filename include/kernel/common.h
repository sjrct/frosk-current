//
// include/boot/common.h
// include/kernel/common.h
//

#ifndef _COMMON_H_
#define _COMMON_H_

#define define(X, Y) enum { X = Y };

#include "common.both"

#ifdef __ARCH_X86_64
	#include "x86_64/common.both"
#else
	#include "i386/common.both"
#endif

#undef define

#endif
