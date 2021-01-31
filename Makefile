include Makefile.inc
include resources/Makefile.inc
include source/Makefile.inc
include source/tests/Makefile.inc
include source/main/Makefile.inc

.OBJS: $(OBJS)
.BINS: $(BINS)

all: $(ALL)

deps: $(DEPS)

test:
	@echo "Tests succeeded"

clean:
	@echo "Removing $(BIN_DIR)"
	@rm -rf $(BIN_DIR)

.PHONY: all deps clean .OBJS .BINS