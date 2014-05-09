#
# Makefile
#

-include estab.mk
include $(BLDD)/local.mk

TARGET := $(CD)/frosk.img

SUB    := util boot kernel prgm
SUBTR  := util/util.fake boot/boot.bin kernel/kernel.bin prgm/prgm.fake

SUBMK  := $(foreach S,$(SUB),$(CD)/$S/Makefile)
SUBTR  := $(foreach S,$(SUBTR),$(CD)/$S)
SUBCL  := $(foreach T,$(SUBTR),$T/clean)

define block
$(TARGET): $(SUBTR) default.fsd
	$$(call V1,$(FSB) -idefault.fsd -o$$@,FSB $$@)
	$$(call V1,dd if=boot/boot.bin of=$$@ conv=notrunc 2> /dev/null,DD $$@)

.PHONY: $(TARGET)/clean
$(TARGET)/clean: $(SUBCL)
	$$(call V1,$(RM) $(TARGET),RM $(TARGET))
endef

$(eval $(block))
include $(SUBMK)
