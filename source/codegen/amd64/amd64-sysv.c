#include <stdio.h>
#include <stdarg.h>
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/codegen/amd64/labels.h"
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/instr.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t cg_declare_opcode_handler(kefir_iropcode_t opcode, const char *handler, void *payload) {
    UNUSED(opcode);
    struct kefir_amd64_asmgen *asmgen = (struct kefir_amd64_asmgen *) payload;
    ASMGEN_EXTERNAL(asmgen, handler);
    return KEFIR_OK;
}

static kefir_result_t cg_module_prologue(const struct kefir_ir_module *module,
                                       struct kefir_codegen_amd64 *codegen) {
    ASMGEN_SECTION(&codegen->asmgen, ".text");
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_COMMENT0(&codegen->asmgen, "Opcode handlers");
    REQUIRE_OK(kefir_amd64_iropcode_handler_list(cg_declare_opcode_handler, &codegen->asmgen));
    ASMGEN_COMMENT0(&codegen->asmgen, "Runtime functions");
    for (kefir_size_t i = 0; i < KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOL_COUNT; i++) {
        ASMGEN_EXTERNAL(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[i]);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    struct kefir_list_entry *iter = NULL;
    ASMGEN_COMMENT0(&codegen->asmgen, "Externals");
    for (const char *external = kefir_ir_module_externals_iter(module, &iter);
        external != NULL;
        external = kefir_ir_module_named_symbol_iter_next((const struct kefir_list_entry **) &iter)) {
        ASMGEN_EXTERNAL(&codegen->asmgen, external);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_COMMENT0(&codegen->asmgen, "Globals");
    for (const char *global = kefir_ir_module_globals_iter(module, &iter);
        global != NULL;
        global = kefir_ir_module_named_symbol_iter_next((const struct kefir_list_entry **) &iter)) {
        ASMGEN_GLOBAL(&codegen->asmgen, "%s", global);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_function_prologue(struct kefir_codegen_amd64 *codegen,
                                         const struct kefir_amd64_sysv_function *func) {
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_LABEL, func->func->declaration->identifier);
    REQUIRE_OK(kefir_amd64_sysv_function_prologue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_function_epilogue(struct kefir_codegen_amd64 *codegen,
                                         const struct kefir_amd64_sysv_function *func) {
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, func->func->declaration->identifier);
    REQUIRE_OK(kefir_amd64_sysv_function_epilogue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_function_body(struct kefir_codegen_amd64 *codegen,
                                     const struct kefir_amd64_sysv_function *sysv_func) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_PROGRAM_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, sysv_func->func->declaration->identifier);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_SYSV_ABI_PROGRAM_REG);
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, sysv_func->func->declaration->identifier);
    const struct kefir_irinstr *instr = NULL;
    for (kefir_size_t pc = 0; pc < kefir_irblock_length(&sysv_func->func->body); pc++) {
        instr = kefir_irblock_at(&sysv_func->func->body, pc);
        REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to fetch instruction from IR block"));
        REQUIRE_OK(kefir_amd64_sysv_instruction(codegen, sysv_func, instr));
    }
    return KEFIR_OK;
}

struct function_translator {
    struct kefir_codegen_amd64 *codegen;
};

static kefir_result_t cg_translate_function(const struct kefir_ir_function *func,
                                          struct kefir_codegen_amd64 *codegen) {
    struct kefir_amd64_sysv_function sysv_func;
    REQUIRE_OK(kefir_amd64_sysv_function_alloc(codegen->mem, func, &sysv_func));
    kefir_result_t res = cg_function_prologue(codegen, &sysv_func);
    REQUIRE_CHAIN(&res, cg_function_body(codegen, &sysv_func));
    REQUIRE_CHAIN(&res, cg_function_epilogue(codegen, &sysv_func));
    kefir_amd64_sysv_function_free(codegen->mem, &sysv_func);
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_translate(struct kefir_codegen *cg_iface, const struct kefir_ir_module *module) {
    REQUIRE(cg_iface != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator interface"));
    REQUIRE(cg_iface->data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg_iface->data;
    REQUIRE_OK(cg_module_prologue(module, codegen));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(module, &iter);
        func != NULL;
        func = kefir_ir_module_function_next(&iter)) {
        REQUIRE_OK(cg_translate_function(func, codegen));
    }
    return KEFIR_OK;
}

static kefir_result_t cg_close(struct kefir_codegen *cg) {
    REQUIRE(cg != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator interface"));
    REQUIRE(cg->data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg->data;
    KEFIR_AMD64_ASMGEN_CLOSE(&codegen->asmgen);
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *codegen, FILE *out, struct kefir_mem *mem) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator pointer"));
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE_OK(kefir_amd64_nasm_gen_init(&codegen->asmgen, out));
    codegen->iface.translate = cg_translate;
    codegen->iface.close = cg_close;
    codegen->iface.data = codegen;
    codegen->mem = mem;
    return KEFIR_OK;
}