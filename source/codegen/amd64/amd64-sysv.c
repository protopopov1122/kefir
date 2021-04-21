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

static kefir_result_t cg_module_prologue(struct kefir_codegen_amd64_sysv_module *module,
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
    const struct kefir_list_entry *iter = NULL;
    ASMGEN_COMMENT0(&codegen->asmgen, "Externals");
    for (const char *external = kefir_ir_module_externals_iter(module->module, &iter);
        external != NULL;
        external = kefir_ir_module_symbol_iter_next((const struct kefir_list_entry **) &iter)) {
        ASMGEN_EXTERNAL(&codegen->asmgen, external);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_COMMENT0(&codegen->asmgen, "Globals");
    for (const char *global = kefir_ir_module_globals_iter(module->module, &iter);
        global != NULL;
        global = kefir_ir_module_symbol_iter_next((const struct kefir_list_entry **) &iter)) {
        ASMGEN_GLOBAL(&codegen->asmgen, "%s", global);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_function_prologue(struct kefir_codegen_amd64 *codegen,
                                         const struct kefir_amd64_sysv_function *func) {
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_LABEL, func->func->declaration->name);
    REQUIRE_OK(kefir_amd64_sysv_function_prologue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_function_epilogue(struct kefir_codegen_amd64 *codegen,
                                         const struct kefir_amd64_sysv_function *func) {
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, func->func->declaration->name);
    REQUIRE_OK(kefir_amd64_sysv_function_epilogue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_function_body(struct kefir_mem *mem,
                                     struct kefir_codegen_amd64 *codegen,
                                     struct kefir_codegen_amd64_sysv_module *sysv_module,
                                     struct kefir_amd64_sysv_function *sysv_func) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_PROGRAM_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, sysv_func->func->declaration->name);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_SYSV_ABI_PROGRAM_REG);
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, sysv_func->func->declaration->name);
    const struct kefir_irinstr *instr = NULL;
    for (kefir_size_t pc = 0; pc < kefir_irblock_length(&sysv_func->func->body); pc++) {
        instr = kefir_irblock_at(&sysv_func->func->body, pc);
        REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to fetch instruction from IR block"));
        REQUIRE_OK(kefir_amd64_sysv_instruction(mem, codegen, sysv_func, sysv_module, instr));
    }
    ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&codegen->asmgen,
        KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL,
        sysv_func->func->declaration->name);
    ASMGEN_ARG0(&codegen->asmgen, "0");
    return KEFIR_OK;
}

struct function_translator {
    struct kefir_codegen_amd64 *codegen;
};

static kefir_result_t cg_translate_function(struct kefir_mem *mem,
                                          const struct kefir_ir_function *func,
                                          struct kefir_codegen_amd64_sysv_module *sysv_module,
                                          struct kefir_codegen_amd64 *codegen) {
    struct kefir_amd64_sysv_function sysv_func;
    REQUIRE_OK(kefir_amd64_sysv_function_alloc(mem, sysv_module, func, &sysv_func));
    kefir_result_t res = cg_function_prologue(codegen, &sysv_func);
    REQUIRE_CHAIN(&res, cg_function_body(mem, codegen, sysv_module, &sysv_func));
    REQUIRE_CHAIN(&res, cg_function_epilogue(codegen, &sysv_func));
    struct kefir_hashtree_node_iterator appendix_iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&sysv_func.appendix, &appendix_iter);
        node != NULL;
        node = kefir_hashtree_next(&appendix_iter)) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_function_appendix_data *, appendix,
            node->value);
        REQUIRE_OK(appendix->callback(codegen, sysv_module, &sysv_func, (const char *) node->key, appendix->payload));
    }
    kefir_amd64_sysv_function_free(mem, &sysv_func);
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_translate_function_gates(struct kefir_codegen_amd64 *codegen,
                                      const struct kefir_hashtree *tree,
                                      bool virtualDecl) {
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(tree, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_function_decl *, sysv_decl,
            node->value);
        if (virtualDecl) {
            ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_LABEL, sysv_decl->decl->id);
        } else {
            ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_GATE_LABEL, sysv_decl->decl->id);
        }
        REQUIRE_OK(kefir_amd64_sysv_function_invoke(codegen, sysv_decl, virtualDecl));
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
        ASMGEN_NEWLINE(&codegen->asmgen, 1);
    }
    return KEFIR_OK;
}

static kefir_result_t cg_translate_data(struct kefir_mem *mem,
                                      struct kefir_codegen_amd64 *codegen,
                                      struct kefir_codegen_amd64_sysv_module *module) {
    bool first = true;
    struct kefir_hashtree_node_iterator iter;
    const char *identifier = NULL;
    for (const struct kefir_ir_data *data = kefir_ir_module_named_data_iter(module->module, &iter, &identifier);
        data != NULL;
        data = kefir_ir_module_named_data_next(&iter, &identifier)) {
        if (first) {
            ASMGEN_SECTION(&codegen->asmgen, ".data");
            first = false;
        }
        REQUIRE_OK(kefir_amd64_sysv_static_data(mem, codegen, data, identifier));
    }

    kefir_id_t id;
    const char *content = NULL;
    kefir_size_t length = 0;
    kefir_result_t res = KEFIR_OK;
    for (res = kefir_ir_module_string_literal_iter(module->module, &iter, &id, &content, &length);
        res == KEFIR_OK;
        res = kefir_ir_module_string_literal_next(&iter, &id, &content, &length)) {
        if (first) {
            ASMGEN_SECTION(&codegen->asmgen, ".data");
            first = false;
        }

        ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL, id);
        ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_BYTE);
        ASMGEN_STRING_LITERAL(&codegen->asmgen, content, length);
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);
    return KEFIR_OK;
}

static kefir_result_t cg_translate(struct kefir_mem *mem, struct kefir_codegen *cg_iface, const struct kefir_ir_module *module) {
    REQUIRE(cg_iface != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator interface"));
    REQUIRE(cg_iface->data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg_iface->data;
    struct kefir_codegen_amd64_sysv_module sysv_module;
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_alloc(mem, &sysv_module, module));
    REQUIRE_OK(cg_module_prologue(&sysv_module, codegen));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(sysv_module.module, &iter);
        func != NULL;
        func = kefir_ir_module_function_next(&iter)) {
        REQUIRE_OK(cg_translate_function(mem, func, &sysv_module, codegen));
    }
    REQUIRE_OK(cg_translate_function_gates(codegen, &sysv_module.function_gates, false));
    REQUIRE_OK(cg_translate_function_gates(codegen, &sysv_module.function_vgates, true));
    REQUIRE_OK(cg_translate_data(mem, codegen, &sysv_module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_free(mem, &sysv_module));
    return KEFIR_OK;
}

static kefir_result_t cg_close(struct kefir_codegen *cg) {
    REQUIRE(cg != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator interface"));
    REQUIRE(cg->data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg->data;
    KEFIR_AMD64_ASMGEN_CLOSE(&codegen->asmgen);
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *codegen, FILE *out) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator pointer"));
    REQUIRE_OK(kefir_amd64_nasm_gen_init(&codegen->asmgen, out));
    codegen->iface.translate = cg_translate;
    codegen->iface.close = cg_close;
    codegen->iface.data = codegen;
    return KEFIR_OK;
}