#
# Makefile
#

-include estab.mk
include $(TOPD)/local.mk

TARGET := $(CD)/frosk.img
SUB    := util boot
SUBTR  := util/util.fake boot/boot.bin
SUBMK  := $(foreach S,$(SUB),$(CD)/$S/Makefile)
SUBTR  := $(foreach S,$(SUBTR),$(CD)/$S)
SUBCL  := $(foreach T,$(SUBTR),$T/clean)

define block
$(TARGET): $(SUBTR)

.PHONY: $(TARGET)/clean
$(TARGET)/clean: $(SUBCL)
	$(call V1,$(RM) $(TARGET),RM $(TARGET))
endef

$(eval $(block))
include $(SUBMK)
