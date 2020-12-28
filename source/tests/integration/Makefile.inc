KEFIR_INTEGRATION_TESTS_SOURCES := $(wildcard \
	$(SOURCE_DIR)/tests/integration/*.test.c)
KEFIR_INTEGRATION_TEST_SRC := $(KEFIR_INTEGRATION_TESTS_SOURCES)
KEFIR_INTEGRATION_TEST_SRC += $(SOURCE_DIR)/tests/int_test.c
KEFIR_INTEGRATION_TEST_DEPS := $(KEFIR_INTEGRATION_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_INTEGRATION_TEST_OBJS := $(KEFIR_INTEGRATION_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)
KEFIR_INTEGRATION_TESTS := $(KEFIR_INTEGRATION_TESTS_SOURCES:$(SOURCE_DIR)/tests/integration/%.c=$(BIN_DIR)/tests/integration/%)

DEPS += $(KEFIR_INTEGRATION_TEST_DEPS)
OBJS += $(KEFIR_INTEGRATION_TEST_OBJS)

$(BIN_DIR)/tests/integration/%: $(DEPS) $(KEFIR_LIB_OBJS) $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/integration/%.o
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/integration/$(@F).o

integration_tests: $(KEFIR_INTEGRATION_TESTS)
	@$(SOURCE_DIR)/tests/integration/run.sh $^

test: integration_tests

.PHONY: integration_tests