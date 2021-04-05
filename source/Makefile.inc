KEFIR_LIB := $(wildcard \
	$(SOURCE_DIR)/ast/*.c \
	$(SOURCE_DIR)/ast/analyzer/*.c \
	$(SOURCE_DIR)/ast/analyzer/nodes/*.c \
	$(SOURCE_DIR)/ast/constant_expression/*.c \
	$(SOURCE_DIR)/ast/nodes/*.c \
	$(SOURCE_DIR)/ast-translator/*.c \
	$(SOURCE_DIR)/ast-translator/nodes/*.c \
	$(SOURCE_DIR)/ast-translator/scope/*.c \
	$(SOURCE_DIR)/ast/type/*.c \
	$(SOURCE_DIR)/core/*.c \
	$(SOURCE_DIR)/codegen/*.c \
	$(SOURCE_DIR)/codegen/amd64/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/builtins/*.c \
	$(SOURCE_DIR)/ir/*.c \
	$(SOURCE_DIR)/util/*.c)
KEFIR_LIB_DEPS := $(KEFIR_LIB:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_LIB_OBJS := $(KEFIR_LIB:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

DEPS += $(KEFIR_LIB_DEPS)
OBJS += $(KEFIR_LIB_OBJS)