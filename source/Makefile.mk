KEFIR_LIB_SOURCE := $(wildcard \
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
	$(SOURCE_DIR)/util/*.c)

KEFIR_LIB_DEPENDENCIES := $(KEFIR_LIB_SOURCE:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_LIB_OBJECT_FILES := $(KEFIR_LIB_SOURCE:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)
KEFIR_LIB_OBJECT_FILES += $(BIN_DIR)/codegen/amd64/amd64-sysv-runtime-code.s.o

$(BIN_DIR)/codegen/amd64/amd64-sysv-runtime-code.s.o: $(SOURCE_DIR)/runtime/amd64_sysv.s

$(LIBKEFIR_SO): $(KEFIR_LIB_OBJECT_FILES)
	@mkdir -p $(shell dirname "$@")
	$(CC) -shared -o $@ $(KEFIR_LIB_OBJECT_FILES)

DEPENDENCIES += $(KEFIR_LIB_DEPENDENCIES)
OBJECT_FILES += $(KEFIR_LIB_OBJECT_FILES)
BINARIES += $(LIBKEFIR_SO)
