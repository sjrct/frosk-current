#
# build/template.mk
#

ENV_CFLAGS  :=
ENV_ASFLAGS :=
ENV_LDFLAGS :=

# Default the environment programs to the general ones
ENV_CC := $(CC)
ENV_LD := $(LD)

-include $(BLDD)/$(ENV).mk

TARGET := $(CD)/$(TARGET)
COBJS  := $(foreach X,$(CSRCS),$(CD)/obj/$X.o)
CDEPS  := $(foreach X,$(CSRCS),$(CD)/dep/$X.d)
CSRCS  := $(addprefix $(CD)/src/,$(CSRCS))
AOBJS  := $(foreach X,$(ASRCS),$(CD)/obj/$X.o)
ADEPS  := $(foreach X,$(ASRCS),$(CD)/dep/$X.d)
ASRCS  := $(addprefix $(CD)/src/,$(ASRCS))

LIBS := $(foreach X,$(LIBS),$(TOPD)/lib/$X/$X.a)

define block
$(TARGET): $(COBJS) $(AOBJS)
	$$(call V1,$(ENV_LD) $(LDFLAGS) -o $$@ $(COBJS) $(AOBJS) $(LIBS),LD  $$@)

$(CD)/obj/%.c.o: $(CD)/src/%.c
	$(V3) mkdir -p $$(dir $$@)
	$(V3) mkdir -p $$(dir $$(subst /src/,/dep/,$$<))
	$$(call V1,$(ENV_CC) $(CFLAGS) -c -o $$@ $$<,CC  $$@)
	$(V3) $(ENV_CC) $(CFLAGS) -M -MP -MT $$@ -MF $$(subst /src/,/dep/,$$<).d $$<

$(CD)/obj/%.asm.o: $(CD)/src/%.asm
	$(V3) mkdir -p $(CD)/obj
	$(V3) mkdir -p $(CD)/dep
	$$(call V1,$(AS) $(ASFLAGS) -o $$@ $$<,AS  $$@)
	$(V3) $(AS) $(ASFLAGS) -M -MP -MT $$@ $$< > $$(subst /src/,/dep/,$$<).d

.PHONY: $(TARGET)/clean
$(TARGET)/clean:
	$$(call V1,$(RM) -r $(CD)/obj/,RM $(CD)/obj/)
	$$(call V1,$(RM) -r $(CD)/dep/,RM $(CD)/dep/)
	$$(call V1,$(RM) $(TARGET),RM $(TARGET))
endef

$(eval $(block))
-include $(CDEPS) $(ADEPS)
