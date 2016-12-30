builddir ?= build
srcdir ?= .
# This hides the noisy commandline outputs. Show them with "make V=1"
ifneq ($(V),1)
Q ?= @
endif

ASCIIDOC ?= asciidoc
VPATH = $(srcdir) $(builddir)

UNUSED := $(shell mkdir -p $(builddir)/doc $(builddir)/website $(builddir)/html_temp $(builddir)/website/images))

ASCIIDOCS := $(wildcard doc/*.adoc)

HTMLFILES = $(ASCIIDOCS:doc/%.adoc=$(builddir)/html_temp/%)

STATIC_FILES := $(wildcard static/*) $(wildcard static/images/*)

WEBSITE_FILES = $(HTMLFILES:$(builddir)/html_temp/%=$(builddir)/website/%) $(STATIC_FILES:static/%=$(builddir)/website/%)

.SECONDARY:

.PHONY: all
all: vcg-examples_subdir $(builddir)/website/Nodes $(WEBSITE_FILES)

.PHONY: upload
upload:
	cp -rvpu output/* /ben/www/firm

.PHONY: clean
clean:
	@echo 'CLEAN'
	$(Q)rm -rf $(builddir)/*
	$(Q)cd vcg-examples && $(MAKE) clean

.PHONY: vcg-examples_subdir
vcg-examples_subdir:
	@echo "Entering directory 'vcg-examples'"
	$(Q)$(MAKE) -C vcg-examples
	@echo "Leaving directory 'vcg-examples'"

$(builddir)/html_temp/%: doc/%.adoc firm.conf header.html footer.html
	@echo 'ASCIIDOC $@'
	$(Q)$(ASCIIDOC) -b xhtml11 -a source-highlighter=pygments -f firm.conf -o $@ $<
	$(Q)xmllint --noout $@

$(builddir)/website/%: $(builddir)/html_temp/% gentoc.py
	@echo 'GENTOC $@'
	$(Q)./gentoc.py < $< > $@
	$(Q)xmllint --noout $@

$(builddir)/website/%: static/%
	@echo 'CP $@'
	$(Q)cp -p $< $@

.PHONY: firm_make_doc
firm_make_doc:
	$(MAKE) -C $(FIRM_HOME) doc

TAGFILE=$(FIRM_HOME)/build/doc/libfirm.tag
IR_SPEC=$(FIRM_HOME)/scripts/ir_spec.py
$(builddir)/doc/Nodes.adoc: templates/Nodes.adoc templates/docufilters.py $(TAGFILE) | firm_make_doc
	@echo 'GEN_IR $@'
	$(Q)$(FIRM_HOME)/scripts/gen_ir.py -e templates/docufilters.py -Ddoxygen_linkbase=api_latest/ -Ddoxygen_tagfile=$(TAGFILE) $(IR_SPEC) $< > $@

# TODO: Can we autogenerate dependencies?
$(builddir)/doc/Debug_Extension: doc/gdbinit
