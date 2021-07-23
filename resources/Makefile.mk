KEFIR_IR_OPCODES_HEADER_XSL=$(RESOURCES_DIR)/kefir_ir_opcodes_header.xsl
KEFIR_IR_MNEMONIC_HEADER_XSL=$(RESOURCES_DIR)/kefir_ir_mnemonic_header.xsl
KEFIR_IR_MNEMONIC_SOURCE_XSL=$(RESOURCES_DIR)/kefir_ir_mnemonic_source.xsl
KEFIR_IR_FORMAT_IMPL_SOURCE_XSL=$(RESOURCES_DIR)/kefir_ir_format_impl_source.xsl

$(ROOT)/generated/%: $(RESOURCES_DIR)/opcodes.xml
	@mkdir -p $(@D)
	@echo "Generating $@"
	@$(XSLTPROC) $(XSLT_SHEET) $(RESOURCES_DIR)/opcodes.xml > $@
	@$(CLANG_FORMAT) -i --style=file $@

$(ROOT)/generated/kefir_ir_opcodes.h: XSLT_SHEET := $(KEFIR_IR_OPCODES_HEADER_XSL)
$(ROOT)/generated/kefir_ir_opcodes.h: $(KEFIR_IR_OPCODES_HEADER_XSL)
GENERATED_SOURCES += $(ROOT)/generated/kefir_ir_opcodes.h

$(ROOT)/generated/kefir_ir_mnemonic.h: XSLT_SHEET := $(KEFIR_IR_MNEMONIC_HEADER_XSL)
$(ROOT)/generated/kefir_ir_mnemonic.h: $(KEFIR_IR_MNEMONIC_HEADER_XSL)
GENERATED_SOURCES += $(ROOT)/generated/kefir_ir_mnemonic.h

$(ROOT)/generated/kefir_ir_mnemonic.c: XSLT_SHEET := $(KEFIR_IR_MNEMONIC_SOURCE_XSL)
$(ROOT)/generated/kefir_ir_mnemonic.c: $(KEFIR_IR_MNEMONIC_SOURCE_XSL)
GENERATED_SOURCES += $(ROOT)/generated/kefir_ir_mnemonic.c

$(ROOT)/generated/kefir_ir_format_impl_source.c: XSLT_SHEET := $(KEFIR_IR_FORMAT_IMPL_SOURCE_XSL)
$(ROOT)/generated/kefir_ir_format_impl_source.c: $(KEFIR_IR_FORMAT_IMPL_SOURCE_XSL)
GENERATED_SOURCES += $(ROOT)/generated/kefir_ir_format_impl_source.c
