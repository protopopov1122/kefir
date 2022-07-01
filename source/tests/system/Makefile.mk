KEFIR_SYSTEM_TESTS_TEST_SOURCES := $(wildcard \
	$(SOURCE_DIR)/tests/system/codegen/*.gen.c \
	$(SOURCE_DIR)/tests/system/parser/*.gen.c \
	$(SOURCE_DIR)/tests/system/translator/*.gen.c)
KEFIR_SYSTEM_TESTS_ALL_SOURCES = $(KEFIR_SYSTEM_TESTS_TEST_SOURCES)
KEFIR_SYSTEM_TESTS_ALL_SOURCES += $(SOURCE_DIR)/tests/int_test.c
KEFIR_SYSTEM_TESTS_ALL_SOURCES += $(SOURCE_DIR)/tests/util/util.c
KEFIR_SYSTEM_TESTS_DEPENDENCIES := $(KEFIR_SYSTEM_TESTS_ALL_SOURCES:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_SYSTEM_TESTS_OBJECT_FILES := $(KEFIR_SYSTEM_TESTS_ALL_SOURCES:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)
KEFIR_SYSTEM_TESTS_GENERATORS := $(KEFIR_SYSTEM_TESTS_TEST_SOURCES:$(SOURCE_DIR)/tests/system/%.gen.c=$(BIN_DIR)/tests/system/%.gen)
KEFIR_SYSTEM_TESTS_DONE := $(KEFIR_SYSTEM_TESTS_TEST_SOURCES:$(SOURCE_DIR)/tests/system/%.gen.c=$(BIN_DIR)/tests/system/%.test.done)

KEFIR_SYSTEM_TEST_LINKED_LIBS=
ifeq ($(SANITIZE),undefined)
KEFIR_SYSTEM_TEST_LINKED_LIBS=-fsanitize=undefined
endif

$(BIN_DIR)/tests/system/%.gen: $(BIN_DIR)/tests/system/%.gen.o \
							   $(LIBKEFIR_SO) \
							   $(BIN_DIR)/tests/int_test.o \
							   $(BIN_DIR)/tests/util/util.o
	@mkdir -p $(@D)
	@echo "Linking $@"
	@$(CC) -o $@ $(BIN_DIR)/tests/int_test.o $(BIN_DIR)/tests/util/util.o $< $(KEFIR_SYSTEM_TEST_LINKED_LIBS) -L $(LIB_DIR) -lkefir

$(BIN_DIR)/tests/system/%.test.done: $(BIN_DIR)/tests/system/%.gen
	@CC="$(CC)" AS="$(AS)" LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(LIB_DIR) MEMCHECK="$(MEMCHECK)" SANITIZE="$(SANITIZE)" OPT="$(OPT)" DBG="$(DBG)" VALGRIND_OPTIONS="$(VALGRIND_OPTIONS)" CC_TEST_FLAGS="$(CC_TEST_FLAGS)" PLATFORM="$(PLATFORM)" "$(SOURCE_DIR)/tests/system/run.sh" $^
	@touch $@

DEPENDENCIES += $(KEFIR_SYSTEM_TESTS_DEPENDENCIES)
OBJECT_FILES += $(KEFIR_SYSTEM_TESTS_OBJECT_FILES)
BINARIES += $(KEFIR_SYSTEM_TESTS_GENERATORS)
TESTS += $(KEFIR_SYSTEM_TESTS_DONE)