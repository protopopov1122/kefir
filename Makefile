include Makefile.mk
include resources/Makefile.mk
include source/Makefile.mk
include source/tests/Makefile.mk
include source/main/Makefile.mk
include install.mk
include self.mk

.GENERATED_SOURCES: $(GENERATED_SOURCES)
.DEPENDENCIES: $(DEPENDENCIES)
.ASM_FILES: $(ASM_FILES)
.OBJECT_FILES: $(OBJECT_FILES)
.BINARIES: $(BINARIES)
.TEST_BINARIES: $(TEST_BINARIES)
.TEST_RESULTS: $(TEST_RESULTS)
.TESTS: $(TESTS)

all: .BINARIES

test: .TESTS
	@echo "Tests succeeded"

clean:
	@echo "Removing $(BIN_DIR)"
	@rm -rf $(BIN_DIR)

generate: .GENERATED_SOURCES

.PHONY: all test generate clean .GENERATED_SOURCES .DEPENDENCIES .ASM_FILES .OBJECT_FILES .BINARIES .TEST_BINARIES .TEST_RESULTS .TESTS
