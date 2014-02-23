;
; include/boot/common.inc
; include/kernel/common.inc
;

%ifndef _COMMON_INC_
%define _COMMON_INC_

%define define(X, Y) X: equ Y

%include "common.both"

%undef define

%endif
