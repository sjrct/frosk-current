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

# the top directory path
TOPD := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# include the configuration file
include config.mk

# programs to use
CC  := gcc
LD  := gcc
AS  := nasm
RM  := rm -f
FSB := $(TOPD)/util/fsb/fsb

# setup program flags
CONFIG := -DARCH=$(ARCH)
GLOB_CFLAGS  = -DARCH=$(ARCH) -I$(TOPD)/include/$(ENV)/
GLOB_ASFLAGS = -DARCH=$(ARCH) -I$(TOPD)/include/$(ENV)/ -I$(CD)/src/
GLOB_LDFLAGS =

CFLAGS  = $(GLOB_CFLAGS)  $(LOCAL_CFLAGS)
ASFLAGS = $(GLOB_ASFLAGS) $(LOCAL_ASFLAGS)
LDFLAGS = $(GLOB_LDFLAGS) $(LOCAL_LDFLAGS)

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

