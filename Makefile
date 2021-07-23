include Makefile.mk
include resources/Makefile.mk
include source/Makefile.mk
include source/tests/Makefile.mk
include source/main/Makefile.mk

.GENERATED_SOURCES: $(GENERATED_SOURCES)
.DEPENDENCIES: $(DEPENDENCIES)
.OBJECT_FILES: $(OBJECT_FILES)
.BINARIES: $(BINARIES)
.TESTS: $(TESTS)

all: .BINARIES

test: .TESTS
	@echo "Tests succeeded"

clean:
	@echo "Removing $(BIN_DIR)"
	@rm -rf $(BIN_DIR)

generate: .GENERATED_SOURCES

.PHONY: all test clean generate .GENERATED_SOURCES .DEPENDENCIES .OBJECT_FILES .BINARIES .ALL
