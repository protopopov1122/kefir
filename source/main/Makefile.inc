KEFIR_MAIN := $(SOURCE_DIR)/main/main.c
KEFIR_MAIN_DEPS := $(BIN_DIR)/main/main.d
KEFIR_MAIN_OBJS := $(BIN_DIR)/main/main.o

DEPS += $(KEFIR_MAIN_DEPS)
OBJS += $(KEFIR_MAIN_OBJS)
ALL += $(BIN_DIR)/kefir

$(BIN_DIR)/kefir: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_MAIN_OBJS)
	@mkdir -p $(shell dirname "$@")
	@echo "Linking $(shell realpath --relative-to='$(ROOT)' $@)"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_MAIN_OBJS)


all: $(ALL)

deps: $(DEPS)

clean:
	@echo "Removing $(BIN_DIR)"
	@rm -rf $(BIN_DIR)

.PHONY: all deps clean