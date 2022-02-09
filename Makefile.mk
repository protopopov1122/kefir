CC=gcc
AS=as
XSLTPROC=xsltproc
CLANG_FORMAT=clang-format

OPT=-O0
DBG=-g3 -ggdb -DKFT_NOFORK
EXTRAFLAGS=
CFLAGS=-std=c11 -Wall -Wextra -pedantic -Wno-overlength-strings -fPIC $(OPT) $(DBG) $(EXTRAFLAGS)
INCLUDES=-Iheaders
SANFLAGS=
PLATFORM=

ifeq ($(SANITIZE),undefined)
SANFLAGS=-fsanitize=undefined -fno-sanitize-recover=all
CFLAGS+=$(SANFLAGS)
endif

ROOT=.
BIN_DIR=$(ROOT)/bin
LIB_DIR=$(BIN_DIR)/libs
GENERATED_DIR=$(BIN_DIR)/generated
SOURCE_DIR=$(ROOT)/source
HEADERS_DIR=$(ROOT)/headers
RESOURCES_DIR=$(ROOT)/resources
SCRIPTS_DIR=$(ROOT)/scripts
BOOTSTRAP_DIR=$(ROOT)/bootstrap

LIBKEFIR_SO=$(LIB_DIR)/libkefir.so

GENERATED_SOURCES :=
DEPENDENCIES :=
ASM_FILES :=
OBJECT_FILES :=
BINARIES :=
TESTS :=

$(BIN_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(shell dirname "$@")
	@echo "Generating $@"
	@$(CC) $(INCLUDES) -MM -MT '$(@:.d=.o)' $< > $@

$(BIN_DIR)/%.o: $(SOURCE_DIR)/%.c $(BIN_DIR)/%.d
	@mkdir -p $(shell dirname "$@")
	@echo "Building $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/%.s.o: $(SOURCE_DIR)/%.s
	@mkdir -p $(shell dirname "$@")
	@echo "Building $@"
	@$(AS) -o $@ $<