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

$(BIN_DIR)/tests/unit.tests: $(LIBKEFIR_SO) $(KEFIR_UNIT_TEST_OBJECT_FILES)
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_UNIT_TEST_OBJECT_FILES) $(KEFIR_UNIT_TEST_LINKED_LIBS) -L $(LIB_DIR) -lm -lkefir

$(BIN_DIR)/tests/unit.tests.done: $(BIN_DIR)/tests/unit.tests
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(LIB_DIR) VALGRIND_OPTIONS="$(VALGRIND_OPTIONS)" $(SOURCE_DIR)/tests/unit/run.sh $^
	@touch $@

DEPENDENCIES += $(KEFIR_UNIT_TEST_DEPENDENCIES)
OBJECT_FILES += $(KEFIR_UNIT_TEST_OBJECT_FILES)
BINARIES += $(KEFIR_UNIT_TESTS_EXE)
TESTS += $(BIN_DIR)/tests/unit.tests.done
