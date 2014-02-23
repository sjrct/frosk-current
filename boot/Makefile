#
# boot/Makefile
#

-include ../estab.mk
include $(TOPD)/local.mk

TARGET := $(CD)/boot.bin
ENV    := boot

define block
$(TARGET): $(CD)/src/boot.asm
	$(V3) $(AS) $(ASFLAGS) -M -MP -MT $$@ $$< > $(CD)/boot.d
	$$(call V1,$(AS) $(ASFLAGS) -fbin -o $$@ $$<,AS $$@)

.PHONY: $(TARGET)/clean
$(TARGET)/clean:
	$(V3) $(RM) $(CD)/boot.d
	$$(call V1,$(RM) $(TARGET),RM $(TARGET))
endef

$(eval $(block))
-include $(CD)/boot.d

include $(TOPD)/rules.mk
