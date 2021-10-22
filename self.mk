KEFIRCC=$(BIN_DIR)/kefir
KEFIR_SELF_SOURCE := $(wildcard \
	$(SOURCE_DIR)/ast/*.c \
	$(SOURCE_DIR)/ast/analyzer/*.c \
	$(SOURCE_DIR)/ast/analyzer/nodes/*.c \
	$(SOURCE_DIR)/ast/constant_expression/*.c \
	$(SOURCE_DIR)/ast/nodes/*.c \
	$(SOURCE_DIR)/ast-translator/*.c \
	$(SOURCE_DIR)/ast-translator/nodes/*.c \
	$(SOURCE_DIR)/ast-translator/scope/*.c \
	$(SOURCE_DIR)/ast/type/*.c \
	$(SOURCE_DIR)/cli/*.c \
	$(SOURCE_DIR)/core/*.c \
	$(SOURCE_DIR)/codegen/*.c \
	$(SOURCE_DIR)/codegen/amd64/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/builtins/*.c \
	$(SOURCE_DIR)/compiler/*.c \
	$(SOURCE_DIR)/ir/*.c \
	$(SOURCE_DIR)/lexer/*.c \
	$(SOURCE_DIR)/lexer/tokens/*.c \
	$(SOURCE_DIR)/lexer/tokens/string_literal/*.c \
	$(SOURCE_DIR)/parser/*.c \
	$(SOURCE_DIR)/parser/rules/*.c \
	$(SOURCE_DIR)/preprocessor/*.c \
	$(SOURCE_DIR)/util/*.c \
	$(SOURCE_DIR)/main/*.c)

KEFIR_SELF_ASM_FILES := $(KEFIR_SELF_SOURCE:$(SOURCE_DIR)/%.c=$(BIN_DIR)/self/%.s)
KEFIR_SELF_OBJECT_FILES := $(KEFIR_SELF_SOURCE:$(SOURCE_DIR)/%.c=$(BIN_DIR)/self/%.o)
KEFIR_SELF_OBJECT_FILES += $(BIN_DIR)/self/runtime.o
KEFIR_SELF_OBJECT_FILES += $(BIN_DIR)/self/main/help.o

$(BIN_DIR)/self/%.s: $(SOURCE_DIR)/%.c $(KEFIRCC)
	@mkdir -p $(shell dirname "$@")
	@echo "Kefir-Compile $@"
	@$(KEFIRCC) -I $(LIBC_HEADERS) -I $(HEADERS_DIR) -o $@ $<

$(BIN_DIR)/self/%.o: $(BIN_DIR)/self/%.s
	@echo "Assemble $@"
	@$(AS) -o $@ $<

$(BIN_DIR)/self/runtime.o: $(SOURCE_DIR)/runtime/amd64_sysv.s
	@mkdir -p $(shell dirname "$@")
	@echo "Assemble $@"
	@$(AS) -o $@ $<

$(BIN_DIR)/self/main/help.o: $(SOURCE_DIR)/main/help.txt $(SOURCE_DIR)/main/help.s
	@mkdir -p $(shell dirname "$@")
	@echo "Building $@"
	@$(AS) -o $@ $(SOURCE_DIR)/main/help.s

$(BIN_DIR)/self/kefir: $(KEFIR_SELF_OBJECT_FILES)
	@echo "Linking $@"
	@$(LD) -o $@ $^

self: $(BIN_DIR)/self/kefir

ASM_FILES += $(KEFIR_SELF_ASM_FILES)
OBJECT_FILES += $(KEFIR_SELF_OBJECT_FILES)

.PHONY: self