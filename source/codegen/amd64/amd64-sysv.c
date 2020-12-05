#include <stdio.h>
#include <stdarg.h>
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/codegen/amd64/labels.h"
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/core/util.h"

static kefir_result_t cg_symbolic_opcode(kefir_iropcode_t opcode, const char **symbolic) {
    REQUIRE(symbolic != NULL, KEFIR_MALFORMED_ARG);
    *symbolic = kefir_amd64_iropcode_handler(opcode);
    return *symbolic != NULL ? KEFIR_OK : KEFIR_MALFORMED_ARG;
}

static kefir_result_t cg_declare_opcode_handler(kefir_iropcode_t opcode, const char *handler, void *payload) {
    UNUSED(opcode);
    struct kefir_amd64_asmgen *asmgen = (struct kefir_amd64_asmgen *) payload;
    ASMGEN_EXTERNAL(asmgen, handler);
    return KEFIR_OK;
}

static kefir_result_t cg_module_prologue(struct kefir_codegen_amd64 *codegen) {
    ASMGEN_SECTION(&codegen->asmgen, ".text");
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_COMMENT(&codegen->asmgen, "Opcode handlers");
    REQUIRE_OK(kefir_amd64_iropcode_handler_list(cg_declare_opcode_handler, &codegen->asmgen));
    ASMGEN_COMMENT(&codegen->asmgen, "Runtime functions");
    for (kefir_size_t i = 0; i < KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOL_COUNT; i++) {
        ASMGEN_EXTERNAL(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[i]);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_function_prologue(struct kefir_codegen_amd64 *codegen, const struct kefir_irfunction *func) {
    ASMGEN_GLOBAL(&codegen->asmgen, FORMAT(codegen->buf[0], KEFIR_AMD64_SYSV_PROCEDURE_LABEL, func->declaration.identifier));
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_LABEL(&codegen->asmgen, FORMAT(codegen->buf[0], KEFIR_AMD64_SYSV_PROCEDURE_LABEL, func->declaration.identifier));
    REQUIRE_OK(kefir_amd64_sysv_function_prologue(codegen, &func->declaration));
    return KEFIR_OK;
}

static kefir_result_t cg_function_epilogue(struct kefir_codegen_amd64 *codegen, const struct kefir_irfunction *func) {
    UNUSED(func);
    ASMGEN_LABEL(&codegen->asmgen, FORMAT(codegen->buf[0], KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, func->declaration.identifier));
    REQUIRE_OK(kefir_amd64_sysv_function_epilogue(codegen, &func->declaration));
    return KEFIR_OK;
}

static kefir_result_t cg_function_body(struct kefir_codegen_amd64 *codegen, const struct kefir_irfunction *func) {
    ASMGEN_INSTR2(&codegen->asmgen, KEFIR_AMD64_MOV, KEFIR_AMD64_RBX, FORMAT(codegen->buf[0], KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, func->declaration.identifier));
    ASMGEN_INSTR1(&codegen->asmgen, KEFIR_AMD64_JMP, INDIRECT(codegen->buf[0], KEFIR_AMD64_RBX));
    ASMGEN_LABEL(&codegen->asmgen, FORMAT(codegen->buf[0], KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, func->declaration.identifier));
    const struct kefir_irinstr *instr = NULL;
    const char *opcode_symbol = NULL;
    for (kefir_size_t pc = 0; pc < kefir_irblock_length(&func->body); pc++) {
        instr = kefir_irblock_at(&func->body, pc);
        REQUIRE(instr != NULL, KEFIR_UNKNOWN_ERROR);
        REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
        ASMGEN_RAW2(&codegen->asmgen, KEFIR_AMD64_QUAD,
            FORMAT(codegen->buf[0], "%s", opcode_symbol),
            FORMAT(codegen->buf[1], "%li", instr->arg));
    }
    ASMGEN_RAW2(&codegen->asmgen, KEFIR_AMD64_QUAD,
        FORMAT(codegen->buf[0], KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, func->declaration.identifier), "0");
    return KEFIR_OK;
}

static kefir_result_t cg_translate(struct kefir_codegen *cg_iface, const struct kefir_irfunction *func) {
    REQUIRE(cg_iface != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(cg_iface->data != NULL, KEFIR_MALFORMED_ARG);
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg_iface->data;
    REQUIRE_OK(cg_module_prologue(codegen));
    REQUIRE_OK(cg_function_prologue(codegen, func));
    REQUIRE_OK(cg_function_body(codegen, func));
    REQUIRE_OK(cg_function_epilogue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_close(struct kefir_codegen *cg) {
    REQUIRE(cg != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(cg->data != NULL, KEFIR_MALFORMED_ARG);
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg->data;
    KEFIR_AMD64_ASMGEN_CLOSE(&codegen->asmgen);
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *codegen, FILE *out, struct kefir_mem *mem) {
    REQUIRE(codegen != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE_OK(kefir_amd64_nasm_gen_init(&codegen->asmgen, out));
    codegen->iface.translate = cg_translate;
    codegen->iface.close = cg_close;
    codegen->iface.data = codegen;
    codegen->mem = mem;
    return KEFIR_OK;
}