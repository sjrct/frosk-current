#
# kernel/Makefile
#

-include ../estab.mk
include $(TOPD)/local.mk

TARGET := kernel.bin
ENV    := kernel
CSRCS  := kmain.c stubout.c

LOCAL_CFLAGS  := -m64 -ffreestanding -nostdinc -mcmodel=large -mno-red-zone
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS := -m64 -ffreestanding -nostdlib -mcmodel=large -mno-red-zone -T$(CD)/kernel.ld

include $(TOPD)/template.mk
include $(TOPD)/rules.mk
