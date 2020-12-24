CC=gcc
OPT=-O0
DBG=-g3 -ggdb -DKFT_NOFORK
CFLAGS=-std=c11 -Wall -Wextra -pedantic $(OPT) $(DBG)
INCLUDES=-Iheaders

ROOT=.
BIN_DIR=$(ROOT)/bin
SOURCE_DIR=$(ROOT)/source

DEPS :=
OBJS :=
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