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
          mqueue.c \
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
          simd.asm \
          dev/ps2.asm

LOCAL_CFLAGS  := -fno-pic -fno-stack-protector -ffreestanding -nostdinc -DDEBUG -DFROSK_KERNEL -Wno-unused-parameter
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS := -ffreestanding -nostdlib -T$(CD)/kernel.ld

DEBUG_LDFLAGS := -g -no-pie -m64 -ffreestanding -nostdlib -Wl,--build-id=none -T $(CD)/debug.ld

include $(BLDD)/template.mk
include $(BLDD)/rules.mk

define extra
.PHONY: kernel
$(TARGET): $(CD)/kernel.dbg

# Build debug symbols object
$(CD)/kernel.dbg: $(COBJS) $(AOBJS)
	$$(call V1,$(ENV_LD) $$(DEBUG_LDFLAGS) -o $$@ $(COBJS) $(AOBJS),LD  $$@)
endef

$(eval $(extra))
