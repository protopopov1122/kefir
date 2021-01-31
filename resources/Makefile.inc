XSLTPROC=xsltproc

$(HEADERS_DIR)/kefir/ir/opcodes.h: $(RESOURCES_DIR)/kefir_ir_opcodes_header.xsl $(RESOURCES_DIR)/opcodes.xml
	@echo "Generating $@"
	@$(XSLTPROC) $^ > $@

generate: $(HEADERS_DIR)/kefir/ir/opcodes.h

.PHONY: generate