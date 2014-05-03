#
# kernel/Makefile
#

-include ../estab.mk
include $(BLDD)/local.mk

TARGET := kernel.bin
ENV    := kernel
CSRCS  := kmain.c \
          paging.c \
          stubout.c \
          interrupt.c \
          fs/fs.c \
          dev/dev.c \
          dev/pci.c \
          dev/block/ata.c \
          cstd/string.c \
          memory/pages.c \
          memory/kernel.c
ASRCS  := cpuid.asm \
          interrupt.asm

LOCAL_CFLAGS  := -ffreestanding -nostdinc -DDEBUG
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS := -ffreestanding -nostdlib -T$(CD)/kernel.ld

include $(BLDD)/template.mk
include $(BLDD)/rules.mk
