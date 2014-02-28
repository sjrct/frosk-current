;
; include/boot/common.inc
; include/kernel/common.inc
;

%ifndef _COMMON_INC_
%define _COMMON_INC_

%define define(X, Y) X: equ Y

%include "common.both"

%ifdef __ARCH_X86_64
	%include "x86_64/common.both"
%else
	%include "i386/common.both"
%endif

%undef define

%endif
