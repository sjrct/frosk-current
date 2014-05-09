#
# prgm/Makefile
#

-include ../estab.mk
include $(BLDD)/local.mk

TARGET := $(CD)/prgm.fake
PRGM   := start
PRGMMK := $(foreach P,$(PRGM),$(CD)/$P/Makefile)
PRGMTR := $(foreach P,$(PRGM),$(CD)/$P/$P)
PRGMCL := $(foreach T,$(PRGMTR),$T/clean)

define block
$(TARGET): $(PRGMTR)
	$(V3) touch $$@

.PHONY: $(TARGET)/clean
$(TARGET)/clean: $(PRGMCL)
	$(V3) $(RM) $(TARGET)
endef

$(eval $(block))

include $(PRGMMK)

