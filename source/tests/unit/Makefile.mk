KEFIR_UNIT_TEST_SOURCE := $(wildcard \
	$(SOURCE_DIR)/tests/unit_test.c \
	$(SOURCE_DIR)/tests/util/util.c \
	$(SOURCE_DIR)/tests/unit/*.c)
KEFIR_UNIT_TEST_DEPENDENCIES := $(KEFIR_UNIT_TEST_SOURCE:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_UNIT_TEST_OBJECT_FILES := $(KEFIR_UNIT_TEST_SOURCE:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

KEFIR_UNIT_TEST_LINKED_LIBS=
ifeq ($(SANITIZE),undefined)
KEFIR_UNIT_TEST_LINKED_LIBS=-fsanitize=undefined
endif

$(BIN_DIR)/tests/unit.tests: $(GENERATED_SOURCES) $(KEFIR_LIB_OBJECT_FILES) $(KEFIR_UNIT_TEST_OBJECT_FILES)
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJECT_FILES) $(KEFIR_UNIT_TEST_OBJECT_FILES) $(KEFIR_UNIT_TEST_LINKED_LIBS)

$(BIN_DIR)/tests/unit.tests.done: $(BIN_DIR)/tests/unit.tests
	@$(SOURCE_DIR)/tests/unit/run.sh $^
	@touch $@

DEPENDENCIES += $(KEFIR_UNIT_TEST_DEPENDENCIES)
OBJECT_FILES += $(KEFIR_UNIT_TEST_OBJECT_FILES)
BINARIES += $(KEFIR_UNIT_TESTS_EXE)
TESTS += $(BIN_DIR)/tests/unit.tests.done
