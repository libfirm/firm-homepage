UNUSED := $(shell mkdir -p $(builddir)/vcgs)
builddir_vcgs := $(builddir)/vcgs

LINKFLAGS = $(FIRM_HOME)/build/debug/libfirm.a -lm
CPPFLAGS  = -I$(FIRM_HOME)/include -I$(FIRM_HOME)/build/gen/include/libfirm
CFLAGS    = -std=c99 -Wall -W
SOURCES   = makevcgs.c
OBJECTS   = $(SOURCES:%.c=$(builddir_vcgs)/%.o)
VCGS      = $(addprefix $(builddir_vcgs)/, twocalls.vcg member.vcg load_mul.vcg simpleadd.vcg jump.vcg condjump.vcg phi.vcg loop.vcg start_return.vcg params.vcg)
SVGS      = $(VCGS:$(builddir_vcgs)/%.vcg=$(builddir)/website/%.svg)
YCOMP    ?= $(abspath ../../ycomp-1.3.19/ycomp)

.SECONDARY:
.PHONY: all
all: $(SVGS)

$(builddir)/website/%.svg: $(builddir_vcgs)/%.vcg
	@echo YCOMP $@
	$(Q)$(YCOMP) --dolayout --export $@ $<

MAKEVCGS=$(builddir_vcgs)/makevcgs
$(VCGS): $(MAKEVCGS)
	@echo MAKEVCGS $@
	$(Q)cd $(builddir_vcgs) && $(MAKEVCGS)

$(MAKEVCGS): $(OBJECTS)
	@echo LINK $@
	$(Q)$(CC) $^ $(LINKFLAGS) -o $@

$(builddir_vcgs)/%.o: %.c
	@echo CC
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	@echo CLEAN
	$(Q)rm -rf makevcgs $(wildcard *.vcg) $(OBJECTS) $(SVGS)
