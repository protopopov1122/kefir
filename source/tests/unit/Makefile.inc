KEFIR_UNIT_TEST_SRC := $(wildcard \
	$(SOURCE_DIR)/tests/unit_test.c \
	$(SOURCE_DIR)/tests/unit/*.c)
KEFIR_UNIT_TEST_DEPS := $(KEFIR_UNIT_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_UNIT_TEST_OBJS := $(KEFIR_UNIT_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

DEPS += $(KEFIR_UNIT_TEST_DEPS)
OBJS += $(KEFIR_UNIT_TEST_OBJS)

$(BIN_DIR)/tests/unit.tests: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)

unit_tests: $(BIN_DIR)/tests/unit.tests
	@$(SOURCE_DIR)/tests/unit/run.sh $^

test: unit_tests

.PHONY: unit_tests