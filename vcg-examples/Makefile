-include config.mak

FIRM_HOME ?= "../../libfirm"

LINKFLAGS = $(FIRM_HOME)/build/debug/libfirm.a -lm
CPPFLAGS  = -I$(FIRM_HOME)/include -I$(FIRM_HOME)/build/gen/include/libfirm
CFLAGS    = -std=c99 -Wall -W
SOURCES   = makevcgs.c
OBJECTS   = $(SOURCES:%.c=%.o)
VCGS      = twocalls.vcg member.vcg load_mul.vcg simpleadd.vcg jump.vcg condjump.vcg phi.vcg loop.vcg start_return.vcg params.vcg
SVGS      = $(VCGS:%.vcg=../build/website/%.svg)
YCOMP    ?= ycomp

Q?=@

.PHONY: clean

all: $(SVGS)

../build/website/%.svg: %.vcg
	@echo YCOMP $@
	$(Q)$(YCOMP) --dolayout --export $@ $<

$(VCGS): makevcgs
	@echo MAKEVCGS $@
	$(Q)./makevcgs

makevcgs: $(OBJECTS)
	@echo LINK $@
	$(Q)$(CC) $^ $(LINKFLAGS) -o $@

%.o: %.c
	@echo CC
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	@echo CLEAN
	$(Q)rm -rf makevcgs $(wildcard *.vcg) $(OBJECTS) $(SVGS)
