KEFIR_STANDALONE_DEPENDENCIES := $(BIN_DIR)/main/standalone.d $(BIN_DIR)/main/runner.d
KEFIR_STANDALONE_OBJECT_FILES := $(BIN_DIR)/main/standalone.o $(BIN_DIR)/main/runner.o $(BIN_DIR)/main/help.s.o

KEFIR_STANDALONE_LINKED_LIBS=
ifeq ($(SANITIZE),undefined)
KEFIR_STANDALONE_LINKED_LIBS=-fsanitize=undefined
endif

$(BIN_DIR)/main/help.s.o: $(SOURCE_DIR)/main/help.txt

$(BIN_DIR)/kefir: $(KEFIR_STANDALONE_OBJECT_FILES) $(LIBKEFIR_SO)
	@mkdir -p $(shell dirname "$@")
	$(CC) -o $@ $(KEFIR_STANDALONE_OBJECT_FILES) $(KEFIR_STANDALONE_LINKED_LIBS) -L $(LIB_DIR) -lkefir

DEPENDENCIES += $(KEFIR_STANDALONE_DEPENDENCIES)
OBJECT_FILES += $(KEFIR_STANDALONE_OBJECT_FILES)
BINARIES += $(BIN_DIR)/kefir
