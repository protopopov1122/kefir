XSLTPROC=xsltproc

$(HEADERS_DIR)/kefir/ir/opcodes.h: $(RESOURCES_DIR)/kefir_ir_opcodes_header.xsl $(RESOURCES_DIR)/opcodes.xml
	@echo "Generating $@"
	@$(XSLTPROC) $^ > $@

$(HEADERS_DIR)/kefir/ir/mnemonic.h: $(RESOURCES_DIR)/kefir_ir_mnemonic_header.xsl $(RESOURCES_DIR)/opcodes.xml
	@echo "Generating $@"
	@$(XSLTPROC) $^ > $@

$(SOURCE_DIR)/ir/mnemonic.c: $(RESOURCES_DIR)/kefir_ir_mnemonic_source.xsl $(RESOURCES_DIR)/opcodes.xml
	@echo "Generating $@"
	@$(XSLTPROC) $^ > $@

$(SOURCE_DIR)/ir/format_impl.c: $(RESOURCES_DIR)/kefir_ir_format_impl_source.xsl $(RESOURCES_DIR)/opcodes.xml
	@echo "Generating $@"
	@$(XSLTPROC) $^ > $@

generate: $(HEADERS_DIR)/kefir/ir/opcodes.h \
          $(HEADERS_DIR)/kefir/ir/mnemonic.h \
		  $(SOURCE_DIR)/ir/mnemonic.c \
		  $(SOURCE_DIR)/ir/format_impl.c

.PHONY: generate