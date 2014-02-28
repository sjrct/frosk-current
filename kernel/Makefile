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

LOCAL_CFLAGS  := -ffreestanding -nostdinc
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS := -ffreestanding -nostdlib -T$(CD)/kernel.ld

include $(BLDD)/template.mk
include $(BLDD)/rules.mk
