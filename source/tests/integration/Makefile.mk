KEFIR_INTEGRATION_TESTS_SOURCES := $(wildcard \
	$(SOURCE_DIR)/tests/integration/ast/*.test.c \
	$(SOURCE_DIR)/tests/integration/codegen_x86_64/*.test.c)
KEFIR_INTEGRATION_TEST_ALL_SOURCES := $(KEFIR_INTEGRATION_TESTS_SOURCES)
KEFIR_INTEGRATION_TEST_ALL_SOURCES += $(SOURCE_DIR)/tests/int_test.c
KEFIR_INTEGRATION_TEST_DEPENDENCIES := $(KEFIR_INTEGRATION_TEST_ALL_SOURCES:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_INTEGRATION_TEST_OBJECT_FILES := $(KEFIR_INTEGRATION_TEST_ALL_SOURCES:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)
KEFIR_INTEGRATION_TEST_BINARIES := $(KEFIR_INTEGRATION_TESTS_SOURCES:$(SOURCE_DIR)/tests/integration/%.c=$(BIN_DIR)/tests/integration/%)
KEFIR_INTEGRATION_TEST_DONE := $(KEFIR_INTEGRATION_TESTS_SOURCES:$(SOURCE_DIR)/tests/integration/%.c=$(BIN_DIR)/tests/integration/%.done)
KEFIR_INTEGRATION_TEST_RESULTS := $(KEFIR_INTEGRATION_TESTS_SOURCES:$(SOURCE_DIR)/tests/integration/%.test.c=$(SOURCE_DIR)/tests/integration/%.test.result)

KEFIR_INTEGRATION_TEST_LINKED_LIBS=
ifeq ($(SANITIZE),undefined)
KEFIR_INTEGRATION_TEST_LINKED_LIBS=-fsanitize=undefined
endif

$(BIN_DIR)/tests/integration/%: $(BIN_DIR)/tests/integration/%.o $(LIBKEFIR_SO) \
                                     $(BIN_DIR)/tests/int_test.o \
									 $(BIN_DIR)/tests/util/util.o
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(BIN_DIR)/tests/int_test.o \
	                             $(BIN_DIR)/tests/util/util.o \
								 $< \
								 $(KEFIR_INTEGRATION_TEST_LINKED_LIBS) \
								 -L $(LIB_DIR) -lkefir

$(BIN_DIR)/tests/integration/%.done: $(BIN_DIR)/tests/integration/%
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(LIB_DIR) $(SOURCE_DIR)/tests/integration/run.sh $<
	@touch $@

$(SOURCE_DIR)/tests/integration/%.test.result: $(BIN_DIR)/tests/integration/%.test
	@echo "Rebuilding $@"
	$^ > $@

rebuild_integration_tests: $(KEFIR_INTEGRATION_TEST_RESULTS)

DEPENDENCIES += $(KEFIR_INTEGRATION_TEST_DEPENDENCIES)
OBJECT_FILES += $(KEFIR_INTEGRATION_TEST_OBJECT_FILES)
BINARIES += $(KEFIR_INTEGRATION_TEST_BINARIES)
TESTS += $(KEFIR_INTEGRATION_TEST_DONE)

.PHONY: rebuild_integration_tests