KEFIR_UNIT_TEST_SRC := $(wildcard \
	$(SOURCE_DIR)/tests/unit_test.c \
	$(SOURCE_DIR)/tests/unit/*.c)
KEFIR_UNIT_TEST_DEPS := $(KEFIR_UNIT_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_UNIT_TEST_OBJS := $(KEFIR_UNIT_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

KEFIR_INTEGRATION_TESTS_SOURCES := $(wildcard \
	$(SOURCE_DIR)/tests/integration/*.c)
KEFIR_INTEGRATION_TEST_SRC := $(KEFIR_INTEGRATION_TESTS_SOURCES)
KEFIR_INTEGRATION_TEST_SRC += $(SOURCE_DIR)/tests/int_test.c
KEFIR_INTEGRATION_TEST_DEPS := $(KEFIR_INTEGRATION_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_INTEGRATION_TEST_OBJS := $(KEFIR_INTEGRATION_TEST_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)
KEFIR_INTEGRATION_TESTS := $(KEFIR_INTEGRATION_TESTS_SOURCES:$(SOURCE_DIR)/tests/integration/%.c=$(BIN_DIR)/tests/integration/%)

DEPS += $(KEFIR_UNIT_TEST_DEPS) $(KEFIR_INTEGRATION_TEST_DEPS)
OBJS += $(KEFIR_UNIT_TEST_OBJS) $(KEFIR_INTEGRATION_TEST_OBJS)

$(BIN_DIR)/tests/unit.tests: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)

$(BIN_DIR)/tests/integration/%: $(DEPS) $(KEFIR_LIB_OBJS) $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/integration/%.o
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/integration/$(@F).o

unit_tests: $(BIN_DIR)/tests/unit.tests
	@$(SOURCE_DIR)/tests/unit/run.sh $^

integration_tests: $(KEFIR_INTEGRATION_TESTS)
	@$(SOURCE_DIR)/tests/integration/run.sh $^

test: unit_tests integration_tests

.PHONY: unit_tests integration_tests