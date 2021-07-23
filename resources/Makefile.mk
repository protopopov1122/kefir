KEFIR_IR_OPCODES_HEADER_XSL=$(RESOURCES_DIR)/kefir_ir_opcodes_header.xsl
KEFIR_IR_MNEMONIC_HEADER_XSL=$(RESOURCES_DIR)/kefir_ir_mnemonic_header.xsl
KEFIR_IR_MNEMONIC_SOURCE_XSL=$(RESOURCES_DIR)/kefir_ir_mnemonic_source.xsl
KEFIR_IR_FORMAT_IMPL_SOURCE_XSL=$(RESOURCES_DIR)/kefir_ir_format_impl_source.xsl

$(HEADERS_DIR)/kefir/ir/opcodes.h: $(RESOURCES_DIR)/opcodes.xml
	@mkdir -p $(@D)
	@echo "Generating $@"
	@$(XSLTPROC) $(KEFIR_IR_OPCODES_HEADER_XSL) $(RESOURCES_DIR)/opcodes.xml > $@
	@$(CLANG_FORMAT) -i --style=file $@

$(HEADERS_DIR)/kefir/ir/mnemonic.h: $(RESOURCES_DIR)/opcodes.xml
	@mkdir -p $(@D)
	@echo "Generating $@"
	@$(XSLTPROC) $(KEFIR_IR_MNEMONIC_HEADER_XSL) $(RESOURCES_DIR)/opcodes.xml > $@
	@$(CLANG_FORMAT) -i --style=file $@

$(SOURCE_DIR)/ir/mnemonic.c: $(RESOURCES_DIR)/opcodes.xml $(HEADERS_DIR)/kefir/ir/mnemonic.h $(HEADERS_DIR)/kefir/ir/opcodes.h
	@mkdir -p $(@D)
	@echo "Generating $@"
	@$(XSLTPROC) $(KEFIR_IR_MNEMONIC_SOURCE_XSL) $(RESOURCES_DIR)/opcodes.xml > $@
	@$(CLANG_FORMAT) -i --style=file $@

$(SOURCE_DIR)/ir/format_impl.c: $(RESOURCES_DIR)/opcodes.xml
	@mkdir -p $(@D)
	@echo "Generating $@"
	@$(XSLTPROC) $(KEFIR_IR_FORMAT_IMPL_SOURCE_XSL) $(RESOURCES_DIR)/opcodes.xml > $@
	@$(CLANG_FORMAT) -i --style=file $@
