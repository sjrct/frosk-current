#
# lib/frosk/Makefile
#

-include ../../estab.mk
include $(BLDD)/local.mk

TARGET := cstd
ENV    := lib
CSRCS  := ctype.c string.c malloc.c stdio.c stdlib.c time.c
LOCAL_CFLAGS := -Wno-unused-parameter

include $(BLDD)/ar-template.mk
include $(BLDD)/rules.mk
