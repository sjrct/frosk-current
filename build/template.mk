#
# util/template.mk
#

ENV_CFLAGS  :=
ENV_ASFLAGS :=
ENV_LDFLAGS :=

-include $(BLDD)/$(ENV).mk

TARGET := $(CD)/$(TARGET)
COBJS  := $(foreach X,$(CSRCS),$(CD)/obj/$X.o)
CDEPS  := $(foreach X,$(CSRCS),$(CD)/dep/$X.d)
CSRCS  := $(addprefix $(CD)/src/,$(CSRCS))
AOBJS  := $(foreach X,$(ASRCS),$(CD)/obj/$X.o)
ADEPS  := $(foreach X,$(ASRCS),S(CD)/dep/$X.d)
ASRCS  := $(addprefix $(CD)/src/,$(ASRCS))

define block
$(TARGET): $(COBJS) $(AOBJS)
	$$(call V1,$(LD) $(LDFLAGS) -o $$@ $$^,LD $$@)

$(CD)/obj/%.c.o: $(CD)/src/%.c
	$(V3) mkdir -p $$(dir $$@)
	$(V3) mkdir -p $$(dir $$(subst /src/,/dep/,$$<))
	$$(call V1,$(CC) $(CFLAGS) -c -o $$@ $$<,CC $$@)
	$(V3) $(CC) $(CFLAGS) -M -MP -MT $$@ -MF $$(subst /src/,/dep/,$$<).d $$<

$(CD)/obj/%.asm.o: $(CD)/src/%.asm
	$(V3) mkdir -p $(CD)/obj
	$(V3) mkdir -p $(CD)/dep
	$$(call V1,$(AS) $(ASFLAGS) -o $$@ $$<,AS $$@)
	$(V3) $(AS) $(ASFLAGS) -M -MP -MT $$@ $$< > $$(subst /src/,/dep/,$$<).d

.PHONY: $(TARGET)/clean
$(TARGET)/clean:
	$$(call V1,$(RM) -r $(CD)/obj/,RM $(CD)/obj/)
	$$(call V1,$(RM) -r $(CD)/dep/,RM $(CD)/dep/)
	$$(call V1,$(RM) $(TARGET),RM $(TARGET))
endef

$(eval $(block))
-include $(CDEPS) $(ADEPS)
