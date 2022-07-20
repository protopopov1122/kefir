SOURCE=
HEADERS=
BOOTSTRAP=
KEFIRCC=
PLATFORM=
AS=as
LD=ld

KEFIR_FLAGS=-I $(HEADERS)
ifeq ($(PLATFORM),freebsd)
KEFIR_FLAGS += --target x86_64-freebsd-system
else ifeq ($(PLATFORM),openbsd)
KEFIR_FLAGS += --target x86_64-openbsd-system -include $(HEADERS)/bootstrap/openbsd.h
else ifeq ($(PLATFORM),netbsd)
KEFIR_FLAGS += --target x86_64-netbsd-system -W --feature-skip-assembly -include $(HEADERS)/bootstrap/netbsd.h
else
KEFIR_FLAGS += --target x86_64-linux-gnu
endif

KEFIR_SOURCE := $(wildcard \
	$(SOURCE)/ast/*.c \
	$(SOURCE)/ast/analyzer/*.c \
	$(SOURCE)/ast/analyzer/nodes/*.c \
	$(SOURCE)/ast/constant_expression/*.c \
	$(SOURCE)/ast/nodes/*.c \
	$(SOURCE)/ast-translator/*.c \
	$(SOURCE)/ast-translator/nodes/*.c \
	$(SOURCE)/ast-translator/scope/*.c \
	$(SOURCE)/ast/type/*.c \
	$(SOURCE)/core/*.c \
	$(SOURCE)/codegen/*.c \
	$(SOURCE)/codegen/amd64/*.c \
	$(SOURCE)/codegen/amd64/system-v/*.c \
	$(SOURCE)/codegen/amd64/system-v/abi/*.c \
	$(SOURCE)/codegen/amd64/system-v/abi/builtins/*.c \
	$(SOURCE)/compiler/*.c \
	$(SOURCE)/ir/*.c \
	$(SOURCE)/lexer/*.c \
	$(SOURCE)/lexer/tokens/*.c \
	$(SOURCE)/lexer/tokens/string_literal/*.c \
	$(SOURCE)/parser/*.c \
	$(SOURCE)/parser/rules/*.c \
	$(SOURCE)/platform/*.c \
	$(SOURCE)/preprocessor/*.c \
	$(SOURCE)/util/*.c \
	$(SOURCE)/driver/*.c)

KEFIR_ASM_FILES := $(KEFIR_SOURCE:$(SOURCE)/%.c=$(BOOTSTRAP)/%.s)
KEFIR_ASM_FILES += $(SOURCE)/runtime/amd64_sysv.s
KEFIR_ASM_FILES += $(SOURCE)/driver/help.s
KEFIR_ASM_FILES += $(SOURCE)/codegen/amd64/amd64-sysv-runtime-code.s

$(BOOTSTRAP)/%.s: $(SOURCE)/%.c
	@mkdir -p $(shell dirname "$@")
	@echo "Kefir-Compile $^"
	@KEFIR_AS=$(AS) $(KEFIRCC) $(KEFIR_FLAGS) -S -o $@ $<

$(BOOTSTRAP)/%.s.o: $(SOURCE)/%.s
	@echo "Assemble $^"
	@KEFIR_AS=$(AS) $(KEFIRCC) $(KEFIR_FLAGS) -c -o $@ $<

$(BOOTSTRAP)/runtime.o: $(SOURCE)/runtime/amd64_sysv.s
	@mkdir -p $(shell dirname "$@")
	@echo "Assemble $^"
	@KEFIR_AS=$(AS) $(KEFIRCC) $(KEFIR_FLAGS) -c -o $@ $<

$(BOOTSTRAP)/driver/help.s.o: $(SOURCE)/driver/help.txt

$(BOOTSTRAP)/codegen/amd64/amd64-sysv-runtime-code.s.o: $(SOURCE)/runtime/amd64_sysv.s

$(BOOTSTRAP)/kefir: $(KEFIR_ASM_FILES)
	@echo "Linking $@"
	@KEFIR_LD=$(LD) $(KEFIRCC) $(KEFIR_FLAGS) $^ -o $@

bootstrap: $(BOOTSTRAP)/kefir

.ASM_FILES: $(KEFIR_ASM_FILES)

.PHONY: bootstrap .ASM_FILES .OBJECT_FILES