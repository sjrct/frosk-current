#
# lib/Makefile
#

-include ../estab.mk
include $(BLDD)/local.mk

TARGET := $(CD)/lib.fake
LIB    := frosk cstd
LIBMK  := $(foreach P,$(LIB),$(CD)/$P/Makefile)
LIBTR  := $(foreach P,$(LIB),$(CD)/$P/$P.a)
LIBCL  := $(foreach T,$(LIBTR),$T/clean)

ENV_CFLAGS := -fno-pic -fno-stack-protector
ENV_LDFLAGS :=

define block
$(TARGET): $(LIBTR)
	$(V3) touch $$@

.PHONY: $(TARGET)/clean
$(TARGET)/clean: $(LIBCL)
	$(V3) $(RM) $(TARGET)
endef

$(eval $(block))

include $(LIBMK)
