KEFIR_SYSTEM_TESTS_GEN_SOURCES := $(wildcard \
	$(SOURCE_DIR)/tests/system/*.gen.c)
KEFIR_SYSTEM_TESTS_GEN_SRC = $(KEFIR_SYSTEM_TESTS_GEN_SOURCES)
KEFIR_SYSTEM_TESTS_GEN_SRC += $(SOURCE_DIR)/tests/int_test.c
KEFIR_SYSTEM_TESTS_GEN_DEPS := $(KEFIR_SYSTEM_TESTS_GEN_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_SYSTEM_TESTS_GEN_OBJS := $(KEFIR_SYSTEM_TESTS_GEN_SRC:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)
KEFIR_SYSTEM_TESTS_GENS := $(KEFIR_SYSTEM_TESTS_GEN_SOURCES:$(SOURCE_DIR)/tests/system/%.gen.c=$(BIN_DIR)/tests/system/%.gen)

DEPS += $(KEFIR_SYSTEM_TESTS_GEN_DEPS)
OBJS += $(KEFIR_SYSTEM_TESTS_GEN_OBJS)

$(BIN_DIR)/tests/system/%.gen: $(DEPS) $(KEFIR_LIB_OBJS) $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/system/%.gen.o
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/system/$(@F).o

system_tests: $(KEFIR_SYSTEM_TESTS_GENS)
	@$(SOURCE_DIR)/tests/system/run.sh $^

test: system_tests

.PHONY: system_tests