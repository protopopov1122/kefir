KEFIR_UNIT_TEST := $(wildcard \
	$(SOURCE_DIR)/test/unit_test.c \
	$(SOURCE_DIR)/test/unit_tests/*.c)
KEFIR_UNIT_TEST_DEPS := $(KEFIR_UNIT_TEST:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_UNIT_TEST_OBJS := $(KEFIR_UNIT_TEST:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

KEFIR_INT_TESTS_SOURCE := $(wildcard \
	$(SOURCE_DIR)/test/int_tests/*.c)
KEFIR_INT_TESTS := $(KEFIR_INT_TESTS_SOURCE:$(SOURCE_DIR)/test/int_tests/%.c=$(BIN_DIR)/int_tests/%)

VALGRIND=valgrind --trace-children=yes --leak-check=full --error-exitcode=127

DEPS += $(KEFIR_UNIT_TEST_DEPS)
OBJS += $(KEFIR_UNIT_TEST_OBJS)

unit_tests: $(BIN_DIR)/kefir_unit_test
ifdef MEMCHECK
	@$(VALGRIND) $(BIN_DIR)/kefir_unit_test
else
	@$(BIN_DIR)/kefir_unit_test
endif

int_tests: $(KEFIR_INT_TESTS)
	@$(SOURCE_DIR)/test/int_tests/run.sh $^

test: unit_tests int_tests

$(BIN_DIR)/kefir_unit_test: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)
	@mkdir -p $(shell dirname "$@")
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_UNIT_TEST_OBJS)

$(BIN_DIR)/int_tests/%: $(DEPS) $(KEFIR_LIB_OBJS) $(BIN_DIR)/test/int_tests/%.o
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(BIN_DIR)/test/int_tests/$(@F).o

.PHONY: unit_tests int_tests