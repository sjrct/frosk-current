#
# kernel/Makefile
#

-include ../estab.mk
include $(BLDD)/local.mk

TARGET := kernel.bin
ENV    := kernel
CSRCS  := kmain.c \
          exec.c \
          fault.c \
          paging.c \
          stubout.c \
          interrupt.c \
          scheduler.c \
          fs/fs.c \
          fs/pub.c \
          dev/dev.c \
          dev/pci.c \
          dev/ps2.c \
          dev/block/ata.c \
          dev/video/video.c \
          cstd/string.c \
          memory/pages.c \
          memory/kernel.c \
          memory/region.c
ASRCS  := tss.asm \
          lock.asm \
          cpuid.asm \
          fault.asm \
          paging.asm \
          syscall.asm \
          interrupt.asm \
          scheduler.asm \
          dev/ps2.asm

LOCAL_CFLAGS  := -ffreestanding -nostdinc -DDEBUG -Wno-unused-parameter
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS := -ffreestanding -nostdlib -T$(CD)/kernel.ld

include $(BLDD)/template.mk
include $(BLDD)/rules.mk
