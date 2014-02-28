#
# estab.mk
#
# This file should be the first makefile to be looked at, and looked at only
# once. Included it relative to the makefile's directory in each primary 
# makefile.
#

SHELL := bash

# disable implicit rules
.SUFFIXES:

# directory paths
TOPD := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BLDD := $(TOPD)/build

# include the configuration file
include $(TOPD)/config.mk

# programs to use
CC  := gcc
LD  := gcc
AS  := nasm
RM  := rm -f
FSB := $(TOPD)/util/fsb/fsb

# setup program flags
GLOB_CFLAGS  = -Wall -D__ARCH=$(ARCH) -I$(TOPD)/include/$(ENV)/
GLOB_ASFLAGS = -Wall -D__ARCH=$(ARCH) -I$(TOPD)/include/$(ENV)/ -I$(CD)/src/
GLOB_LDFLAGS =

CFLAGS  = $(GLOB_CFLAGS)  $(ARCH_CFLAGS)  $(LOCAL_CFLAGS)
ASFLAGS = $(GLOB_ASFLAGS) $(ARCH_ASFLAGS) $(LOCAL_ASFLAGS)
LDFLAGS = $(GLOB_LDFLAGS) $(ARCH_LDFLAGS) $(LOCAL_LDFLAGS)

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

