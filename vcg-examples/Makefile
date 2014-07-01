-include config.mak

FIRM_HOME ?= "../../libfirm"

LINKFLAGS = $(FIRM_HOME)/build/debug/libfirm.a -lm
CPPFLAGS  = -I$(FIRM_HOME)/ir/common -I$(FIRM_HOME)/ir/tr -I$(FIRM_HOME)/ir/adt -I$(FIRM_HOME)/ir/ana -I$(FIRM_HOME)/ir/ir -I$(FIRM_HOME)/include -I$(FIRM_HOME)/include/libfirm/adt -I$(FIRM_HOME)/include/libfirm -I$(FIRM_HOME)/build/gen/include/libfirm
CFLAGS    = -std=c99 -Wall -W
SOURCES   = makevcgs.c
OBJECTS   = $(SOURCES:%.c=%.o)
VCGS      = twocalls.vcg member.vcg load_mul.vcg simpleadd.vcg jump.vcg condjump.vcg phi.vcg start_return.vcg params.vcg
SVGS      = $(VCGS:%.vcg=../output/%.svg)
YCOMP    ?= ycomp

Q?=@

.PHONY: clean

all: $(SVGS)

../output/%.svg: %.vcg
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
