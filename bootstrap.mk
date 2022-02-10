SOURCE=
HEADERS=
BOOTSTRAP=
KEFIRCC=
LIBC_HEADERS=
LIBC_LIBS=
LD=ld

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
	$(SOURCE)/cli/*.c \
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
	$(SOURCE)/preprocessor/*.c \
	$(SOURCE)/util/*.c \
	$(SOURCE)/main/*.c)

KEFIR_ASM_FILES := $(KEFIR_SOURCE:$(SOURCE)/%.c=$(BOOTSTRAP)/%.s)
KEFIR_OBJECT_FILES := $(KEFIR_SOURCE:$(SOURCE)/%.c=$(BOOTSTRAP)/%.o)
KEFIR_OBJECT_FILES += $(BOOTSTRAP)/runtime.o
KEFIR_OBJECT_FILES += $(BOOTSTRAP)/main/help.s.o
KEFIR_OBJECT_FILES += $(BOOTSTRAP)/codegen/amd64/amd64-sysv-runtime-code.s.o

$(BOOTSTRAP)/%.s: $(SOURCE)/%.c
	@mkdir -p $(shell dirname "$@")
	@echo "Kefir-Compile $^"
	@$(KEFIRCC) -I $(LIBC_HEADERS) -I $(HEADERS) -o $@ $<

$(BOOTSTRAP)/%.o: $(BOOTSTRAP)/%.s
	@echo "Assemble $^"
	@$(AS) -o $@ $<

$(BOOTSTRAP)/%.s.o: $(SOURCE)/%.s
	@echo "Assemble $^"
	@$(AS) -o $@ $<

$(BOOTSTRAP)/runtime.o: $(SOURCE)/runtime/amd64_sysv.s
	@mkdir -p $(shell dirname "$@")
	@echo "Assemble $^"
	@$(AS) -o $@ $<

$(BOOTSTRAP)/main/help.s.o: $(SOURCE)/main/help.txt

$(BOOTSTRAP)/codegen/amd64/amd64-sysv-runtime-code.s.o: $(SOURCE)/runtime/amd64_sysv.s

$(BOOTSTRAP)/kefir: $(KEFIR_OBJECT_FILES)
	@echo "Linking $@"
	@$(LD) $^ $(LIBC_LIBS)/crt1.o $(LIBC_LIBS)/libc.a -o $@

bootstrap: $(BOOTSTRAP)/kefir

.ASM_FILES: $(KEFIR_ASM_FILES)
.OBJECT_FILES: $(KEFIR_OBJECT_FILES)

.PHONY: bootstrap .ASM_FILES .OBJECT_FILES