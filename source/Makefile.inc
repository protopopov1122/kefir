KEFIR_LIB := $(wildcard \
	$(SOURCE_DIR)/ast/*.c \
	$(SOURCE_DIR)/core/*.c \
	$(SOURCE_DIR)/codegen/*.c \
	$(SOURCE_DIR)/codegen/amd64/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/builtins/*.c \
	$(SOURCE_DIR)/ir/*.c)
KEFIR_LIB_DEPS := $(KEFIR_LIB:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_LIB_OBJS := $(KEFIR_LIB:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

DEPS += $(KEFIR_LIB_DEPS)
OBJS += $(KEFIR_LIB_OBJS)