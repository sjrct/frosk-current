#
# build/local.mk
#
# This file is always included in primary makefiles, after estab.mk
#

# reset local variables
CSRCS :=
ASRCS :=
LIBS  :=

LOCAL_CFLAGS  :=
LOCAL_ASFLAGS :=
LOCAL_LDFLAGS :=

# set the default goal and the clean goal
ifndef LOCAL_MK_INCLUDED
  WHAT_TO_BUILD = $(TARGET)
  WHAT_TO_CLEAN = $(TARGET)/clean
else
  WHAT_TO_BUILD := $(WHAT_TO_BUILD)
  WHAT_TO_CLEAN := $(WHAT_TO_CLEAN)
endif

#
# Re-establish the current directory
#  On the first call to this, the included makefile is the 4th to last (as
#  estab.mk and config.mk was included prior). Otherwise, it is the 2nd
#  (as estab.mk and config.mk were not included prior).
#
TMP := $(MAKEFILE_LIST)
TMP := $(filter-out $(lastword $(TMP)),$(TMP))

ifndef LOCAL_MK_INCLUDED
  LOCAL_MK_INCLUDED := yes
  TMP := $(filter-out $(lastword $(TMP)),$(TMP))
  TMP := $(filter-out $(lastword $(TMP)),$(TMP))
endif

CD := $(realpath $(dir $(lastword $(TMP))))
