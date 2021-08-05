CC=gcc
XSLTPROC=xsltproc
CLANG_FORMAT=clang-format

OPT=-O0
DBG=-g3 -ggdb -DKFT_NOFORK
EXTRAFLAGS=
CFLAGS=-std=c11 -Wall -Wextra -pedantic -fPIC $(OPT) $(DBG) $(EXTRAFLAGS)
INCLUDES=-Iheaders

ifeq ($(SANITIZE),undefined)
CFLAGS+=-fsanitize=undefined -fno-sanitize-recover=all
endif

ROOT=.
BIN_DIR=$(ROOT)/bin
LIB_DIR=$(BIN_DIR)/libs
GENERATED_DIR=$(BIN_DIR)/generated
SOURCE_DIR=$(ROOT)/source
HEADERS_DIR=$(ROOT)/headers
RESOURCES_DIR=$(ROOT)/resources

LIBKEFIR_SO=$(LIB_DIR)/libkefir.so

GENERATED_SOURCES :=
DEPENDENCIES :=
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