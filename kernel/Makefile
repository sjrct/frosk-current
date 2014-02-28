#
# kernel/Makefile
#

-include ../estab.mk
include $(BLDD)/local.mk

TARGET := kernel.bin
ENV    := kernel
CSRCS  := kmain.c \
          pci.c \
          paging.c \
          stubout.c \
          interrupt.c \
          fs/fs.c \
          cstd/string.c \
          memory/pages.c \
          memory/kernel.c
ASRCS  := interrupt.asm

LOCAL_CFLAGS  := -m64 -ffreestanding -nostdinc -mcmodel=large -mno-red-zone
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS := -m64 -ffreestanding -nostdlib -T$(CD)/kernel.ld

include $(BLDD)/template.mk
include $(BLDD)/rules.mk
