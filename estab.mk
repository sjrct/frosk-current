#
# estab.mk
#
# This file should be the first makefile to be looked at, and looked at only
# once. Included it relative to the makefile's directory in each primary
# makefile.
#

SHELL := bash

# Are we building frosk?
FROSK := 1

# disable implicit rules
.SUFFIXES:

# directory paths: current, top, build
# current is the directory of the current makefile
CURD = $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
TOPD := $(CURD)
BLDD := $(TOPD)/build

default = $(if $1,$1,$2)

# include the configuration file
include $(TOPD)/config.mk

# programs to use
CC  := $(call default,$(CC),gcc)
LD  := $(call default,$(LD),gcc)
AS  := $(call default,$(AS),nasm)
RM  := $(call default,$(RM),rm -f)
AR  := $(call default,$(AR),ar rcD)
FSB := $(TOPD)/util/fsb/fsb
HOST_CC := $(call default,$(HOST_CC),gcc)
HOST_LD := $(call default,$(HOST_LD),gcc)

# setup program flags
GLOB_CFLAGS  = -Wall -Wextra -D__ARCH=$(ARCH) -I$(TOPD)/include/$(ENV)/
GLOB_ASFLAGS = -Wall -D__ARCH=$(ARCH) -I$(TOPD)/include/$(ENV)/ -I$(CD)/src/
GLOB_LDFLAGS =

CFLAGS  = $(GLOB_CFLAGS)  $(ARCH_CFLAGS)  $(ENV_CFLAGS)  $(LOCAL_CFLAGS)
ASFLAGS = $(GLOB_ASFLAGS) $(ARCH_ASFLAGS) $(ENV_ASFLAGS) $(LOCAL_ASFLAGS)
LDFLAGS = $(GLOB_LDFLAGS) $(ARCH_LDFLAGS) $(ENV_LDFLAGS) $(LOCAL_LDFLAGS)

ifeq ($(ARCH),x86_64)
  ARCH_CFLAGS  = -D__ARCH_X86_64 -m64 -mcmodel=large -mno-red-zone
  ARCH_ASFLAGS = -D__ARCH_X86_64 -felf64
  ARCH_LDFLAGS = -m64
else
  ifeq ($(ARCH),i386)
    ARCH_CFLAGS  := -D__ARCH_I386 -m32
    ARCH_ASFLAGS := -D__ARCH_I386 -felf
    ARCH_LDFLAGS := -m32
  else
    $(error Unknown target archietecture '$(ARCH)')
  endif
endif

#
# Verbosity settings
#   V3 and V2 are used as prefixes
#   V1 is used as a function (with 'call')
#
V := 1
V1 = $(1)

ifneq ($V,3)
  V3 := @
  ifneq ($V,2)
    V2 := @
    ifneq ($V,1)
      V1 = @ $(1)
    else
      V1 = @ echo " $(subst $(TOPD)/,,$(2))"; $(1)
    endif
  endif
endif

