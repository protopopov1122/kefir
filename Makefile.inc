CC=gcc
OPT=-O0
DBG=-g3 -ggdb -DKFT_NOFORK
EXTRAFLAGS=
CFLAGS=-std=c11 -Wall -Wextra -Wno-newline-eof -pedantic $(OPT) $(DBG) $(EXTRAFLAGS)
INCLUDES=-Iheaders

ifeq ($(SANITIZE),undefined)
CFLAGS+=-fsanitize=undefined -fno-sanitize-recover=all
endif

ROOT=.
BIN_DIR=$(ROOT)/bin
SOURCE_DIR=$(ROOT)/source
HEADERS_DIR=$(ROOT)/headers
RESOURCES_DIR=$(ROOT)/resources

DEPS :=
OBJS :=
BINS :=
ALL :=

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