#
# build/prgm.mk
#
# Makefile specific to the program (prgm) environment
#

ENV_CFLAGS  := -ffreestanding -fno-pic -fno-stack-protector -nostdinc -mcmodel=large -mno-red-zone
ENV_ASFLAGS :=
ENV_LDFLAGS := -ffreestanding -nostdlib -mcmodel=large -mno-red-zone -T$(TOPD)/prgm/prgm.ld
