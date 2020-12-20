CC=gcc
OPT=-O0
DBG=-g3 -ggdb -DKFT_NOFORK
CFLAGS=-std=c11 -Wall -Wextra -pedantic $(OPT) $(DBG)
INCLUDES=-Iheaders

VALGRIND=valgrind --trace-children=yes --leak-check=full --error-exitcode=127

BIN_DIR=bin
SOURCE_DIR=source

KEFIR_LIB := $(wildcard \
	$(SOURCE_DIR)/core/*.c \
	$(SOURCE_DIR)/codegen/*.c \
	$(SOURCE_DIR)/codegen/amd64/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/*.c \
	$(SOURCE_DIR)/codegen/amd64/system-v/abi/*.c \
	$(SOURCE_DIR)/ir/*.c)
KEFIR_LIB_DEPS := $(KEFIR_LIB:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_LIB_OBJS := $(KEFIR_LIB:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

KEFIR_MAIN := $(SOURCE_DIR)/main.c
KEFIR_MAIN_DEPS := $(BIN_DIR)/main.d
KEFIR_MAIN_OBJS := $(BIN_DIR)/main.o

KEFIR_TEST := $(wildcard \
	$(SOURCE_DIR)/test/*.c \
	$(SOURCE_DIR)/test/suite/*.c)
KEFIR_TEST_DEPS := $(KEFIR_TEST:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.d)
KEFIR_TEST_OBJS := $(KEFIR_TEST:$(SOURCE_DIR)/%.c=$(BIN_DIR)/%.o)

SOURCE := $(KEFIR_LIB) $(KEFIR_TEST) $(KEFIR_MAIN)
DEPS := $(KEFIR_LIB_DEPS) $(KEFIR_TEST_DEPS) $(KEFIR_MAIN_DEPS)
OBJS := $(KEFIR_LIB_OBJS) $(KEFIR_TEST_OBJS) $(KEFIR_MAIN_OBJS)

all: $(BIN_DIR)/kefir

test: $(BIN_DIR)/kefir_test
ifdef MEMCHECK
	$(VALGRIND) $(BIN_DIR)/kefir_test
else
	$(BIN_DIR)/kefir_test
endif

deps: $(DEPS)

clean:
	@echo "Removing $(BIN_DIR)"
	@rm -rf $(BIN_DIR)

.PHONY: all test deps clean

$(BIN_DIR)/kefir: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_MAIN_OBJS)
	@mkdir -p $(shell dirname "$@")
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_MAIN_OBJS)

$(BIN_DIR)/kefir_test: $(DEPS) $(KEFIR_LIB_OBJS) $(KEFIR_TEST_OBJS)
	@mkdir -p $(shell dirname "$@")
	@echo "Linking $@"
	@$(CC) -o $@ $(KEFIR_LIB_OBJS) $(KEFIR_TEST_OBJS)

$(BIN_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(shell dirname "$@")
	@echo "Generating $@"
	@$(CC) $(INCLUDES) -MM -MT '$(@:.d=.o)' $< > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(BIN_DIR)/%.o: $(SOURCE_DIR)/%.c $(BIN_DIR)/%.d
	@mkdir -p $(shell dirname "$@")
	@echo "Building $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
