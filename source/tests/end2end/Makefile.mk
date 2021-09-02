END2END_BIN_PATH=$(BIN_DIR)/tests/end2end

$(END2END_BIN_PATH)/runtime.o: $(SOURCE_DIR)/runtime/amd64_sysv.asm
	@mkdir -p $(shell dirname "$@")
	@echo "Assemble $@"
	@nasm -f elf64 -o $@ $<

$(BIN_DIR)/%.kefir.o: $(SOURCE_DIR)/%.kefir.c $(BIN_DIR)/kefir
	@mkdir -p $(shell dirname "$@")
	@echo "Kefir-Compile $@"
	@VALGRIND_OPTIONS="$(VALGRIND_OPTIONS)" $(SOURCE_DIR)/tests/end2end/compile.sh $(BIN_DIR) $< $@

$(END2END_BIN_PATH)/%.test: $(END2END_BIN_PATH)/runtime.o
	@mkdir -p $(shell dirname "$@")
	@echo "Linking $@"
	@$(CC) -no-pie $(SANFLAGS) $(END2END_TEST_OBJS) $(END2END_BIN_PATH)/runtime.o -o $@

$(END2END_BIN_PATH)/%.test.done: $(END2END_BIN_PATH)/%.test
	@echo "Running $<"
	@valgrind $(VALGRIND_OPTIONS) $<
	@touch $@

OBJECT_FILES += $(END2END_BIN_PATH)/runtime.o

include source/tests/end2end/*/Makefile.mk