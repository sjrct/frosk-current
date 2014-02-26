#
# util/Makefile
#

-include ../estab.mk
include $(BLDD)/local.mk

TARGET := $(CD)/util.fake
UTIL   := fsb
UTILMK := $(foreach U,$(UTIL),$(CD)/$U/Makefile)
UTILTR := $(foreach U,$(UTIL),$(CD)/$U/$U)
UTILCL := $(foreach T,$(UTILTR),$T/clean)

define block
$(TARGET): $(UTILTR)
	$(V3) touch $$@

.PHONY: $(TARGET)/clean
$(TARGET)/clean: $(UTILCL)
	$(V3) $(RM) $(TARGET)
endef

$(eval $(block))

include $(UTILMK)

