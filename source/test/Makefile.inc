KEFIR_TEST := $(wildcard \
	$(SOURCE_DIR)/test/*.c \
	$(SOURCE_DIR)/test/unit_tests/*.c)
KEFIR_UNIT_TEST_DEPS := $(KEFIR_TEST:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_UNIT_TEST_OBJS := $(KEFIR_TEST:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

VALGRIND=valgrind --trace-children=yes --leak-check=full --error-exitcode=127

DEPS += $(KEFIR_UNIT_TEST_DEPS)
OBJS += $(KEFIR_UNIT_TEST_OBJS)

test: $(BIN_DIR)/kefir_unit_test
ifdef MEMCHECK
	@$(VALGRIND) $(BIN_DIR)/kefir_unit_test
else
	@$(BIN_DIR)/kefir_unit_test
endif

$(BIN_DIR)/kefir_unit_test: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)
	@mkdir -p $(shell dirname "$@")
	@echo "Linking $(shell realpath --relative-to='$(ROOT)' $@)"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)

.PHONY: test