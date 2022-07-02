/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include "kefir/codegen/amd64/system-v/abi/tls.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t cg_declare_opcode_handler(kefir_iropcode_t opcode, const char *handler, void *payload) {
    UNUSED(opcode);
    struct kefir_amd64_asmgen *asmgen = (struct kefir_amd64_asmgen *) payload;
    ASMGEN_EXTERNAL(asmgen, "%s", handler);
    return KEFIR_OK;
}

static kefir_result_t cg_module_externals(struct kefir_codegen_amd64_sysv_module *module,
                                          struct kefir_codegen_amd64 *codegen) {
    struct kefir_hashtree_node_iterator externals_iter;
    kefir_ir_external_type_t external_type;
    for (const char *external = kefir_ir_module_externals_iter(module->module, &externals_iter, &external_type);
         external != NULL; external = kefir_ir_module_externals_iter_next(&externals_iter, &external_type)) {

        if (!codegen->config->emulated_tls || external_type != KEFIR_IR_EXTERNAL_THREAD_LOCAL) {
            ASMGEN_EXTERNAL(&codegen->asmgen, "%s", external);
        } else {
            ASMGEN_EXTERNAL(&codegen->asmgen, KEFIR_AMD64_EMUTLS_V, external);
        }
    }
    return KEFIR_OK;
}

static kefir_result_t cg_module_prologue(struct kefir_codegen_amd64_sysv_module *module,
                                         struct kefir_codegen_amd64 *codegen) {
    ASMGEN_PROLOGUE(&codegen->asmgen);
    ASMGEN_SECTION(&codegen->asmgen, ".text");
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_COMMENT0(&codegen->asmgen, "Opcode handlers");
    REQUIRE_OK(kefir_amd64_iropcode_handler_list(cg_declare_opcode_handler, &codegen->asmgen));
    ASMGEN_COMMENT0(&codegen->asmgen, "Runtime functions");
    for (kefir_size_t i = 0; i < KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOL_COUNT; i++) {
        ASMGEN_EXTERNAL(&codegen->asmgen, "%s", KEFIR_AMD64_SYSTEM_V_RUNTIME_SYMBOLS[i]);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    const struct kefir_list_entry *iter = NULL;
    ASMGEN_COMMENT0(&codegen->asmgen, "Externals");
    REQUIRE_OK(cg_module_externals(module, codegen));
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    ASMGEN_COMMENT0(&codegen->asmgen, "Globals");
    for (const char *global = kefir_ir_module_globals_iter(module->module, &iter); global != NULL;
         global = kefir_ir_module_globals_iter_next((const struct kefir_list_entry **) &iter)) {
        ASMGEN_GLOBAL(&codegen->asmgen, "%s", global);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_function_prologue(struct kefir_codegen_amd64 *codegen,
                                           const struct kefir_amd64_sysv_function *func) {
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_LABEL, func->func->name);
    REQUIRE_OK(kefir_amd64_sysv_function_prologue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_function_epilogue(struct kefir_codegen_amd64 *codegen,
                                           const struct kefir_amd64_sysv_function *func) {
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, func->func->name);
    REQUIRE_OK(kefir_amd64_sysv_function_epilogue(codegen, func));
    return KEFIR_OK;
}

static kefir_result_t cg_function_body(struct kefir_mem *mem, struct kefir_codegen_amd64 *codegen,
                                       struct kefir_codegen_amd64_sysv_module *sysv_module,
                                       struct kefir_amd64_sysv_function *sysv_func) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_PROGRAM_REG);
    ASMGEN_ARG(&codegen->asmgen, "[" KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL "]", sysv_func->func->name);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_SYSV_ABI_PROGRAM_REG);
    ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL, sysv_func->func->name);
    kefir_size_t instr_width;
    for (kefir_size_t pc = 0; pc < kefir_irblock_length(&sysv_func->func->body); pc += instr_width) {
        REQUIRE_OK(kefir_amd64_sysv_instruction(mem, codegen, sysv_func, sysv_module, &sysv_func->func->body, pc,
                                                &instr_width));
    }
    ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, sysv_func->func->name);
    ASMGEN_ARG0(&codegen->asmgen, "0");
    return KEFIR_OK;
}

struct function_translator {
    struct kefir_codegen_amd64 *codegen;
};

static kefir_result_t cg_translate_function(struct kefir_mem *mem, const struct kefir_ir_function *func,
                                            struct kefir_codegen_amd64_sysv_module *sysv_module,
                                            struct kefir_codegen_amd64 *codegen) {
    struct kefir_amd64_sysv_function sysv_func;
    REQUIRE_OK(kefir_amd64_sysv_function_alloc(mem, sysv_module, func, &sysv_func));
    kefir_result_t res = cg_function_prologue(codegen, &sysv_func);
    REQUIRE_CHAIN(&res, cg_function_body(mem, codegen, sysv_module, &sysv_func));
    REQUIRE_CHAIN(&res, cg_function_epilogue(codegen, &sysv_func));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_function_free(mem, &sysv_func);
        return res;
    });

    struct kefir_hashtree_node_iterator appendix_iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&sysv_func.appendix, &appendix_iter);
         res == KEFIR_OK && node != NULL; node = kefir_hashtree_next(&appendix_iter)) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_function_appendix_data *, appendix, node->value);
        REQUIRE_CHAIN(
            &res, appendix->callback(codegen, sysv_module, &sysv_func, (const char *) node->key, appendix->payload));
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_function_free(mem, &sysv_func);
        return res;
    });

    REQUIRE_OK(kefir_amd64_sysv_function_free(mem, &sysv_func));
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

static kefir_result_t cg_translate_function_gates(struct kefir_codegen_amd64 *codegen,
                                                  const struct kefir_hashtree *tree, bool virtualDecl) {
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(tree, &iter); node != NULL;
         node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_function_decl *, sysv_decl, node->value);
        if (virtualDecl) {
            if (sysv_decl->decl->name == NULL) {
                ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_ID_LABEL, sysv_decl->decl->id);
            } else {
                ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_NAMED_LABEL,
                             sysv_decl->decl->name);
            }
        } else {
            if (sysv_decl->decl->name == NULL || sysv_decl->decl->vararg) {
                ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_GATE_ID_LABEL, sysv_decl->decl->id);
            } else {
                ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_GATE_NAMED_LABEL, sysv_decl->decl->name,
                             sysv_decl->decl->id);
            }
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

static kefir_result_t cg_translate_tls_entries(struct kefir_codegen_amd64 *codegen, const struct kefir_hashtree *tree,
                                               const struct kefir_ir_module *module) {
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(tree, &iter); node != NULL;
         node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier, node->key);

        ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_TLS_ENTRY, identifier);
        REQUIRE_OK(kefir_amd64_sysv_thread_local_reference(codegen, identifier,
                                                           !kefir_ir_module_has_external(module, identifier)));
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
        ASMGEN_NEWLINE(&codegen->asmgen, 1);
    }
    return KEFIR_OK;
}

static kefir_result_t cg_translate_strings(struct kefir_codegen_amd64 *codegen,
                                           struct kefir_codegen_amd64_sysv_module *module) {

    kefir_bool_t first = true;
    struct kefir_hashtree_node_iterator iter;
    kefir_id_t id;
    kefir_ir_string_literal_type_t literal_type;
    kefir_bool_t public;
    const void *content = NULL;
    kefir_size_t length = 0;
    kefir_result_t res = KEFIR_OK;
    for (res =
             kefir_ir_module_string_literal_iter(module->module, &iter, &id, &literal_type, &public, &content, &length);
         res == KEFIR_OK;
         res = kefir_ir_module_string_literal_next(&iter, &id, &literal_type, &public, &content, &length)) {
        if (!public) {
            continue;
        }
        if (first) {
            ASMGEN_SECTION(&codegen->asmgen, ".rodata");
            first = false;
        }

        ASMGEN_LABEL(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL, id);
        switch (literal_type) {
            case KEFIR_IR_STRING_LITERAL_MULTIBYTE:
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_ASCII);
                ASMGEN_STRING_LITERAL(&codegen->asmgen, content, length);
                break;

            case KEFIR_IR_STRING_LITERAL_UNICODE16:
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_WORD);
                for (kefir_size_t i = 0; i < length; i++) {
                    kefir_char16_t chr = ((kefir_char16_t *) content)[i];
                    ASMGEN_ARG(&codegen->asmgen, KEFIR_UINT16_FMT, chr);
                }
                break;

            case KEFIR_IR_STRING_LITERAL_UNICODE32:
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_DOUBLE);
                for (kefir_size_t i = 0; i < length; i++) {
                    kefir_char32_t chr = ((kefir_char32_t *) content)[i];
                    ASMGEN_ARG(&codegen->asmgen, KEFIR_UINT32_FMT, chr);
                }
                break;
        }
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);
    return KEFIR_OK;
}

static kefir_result_t cg_translate_data_storage(struct kefir_mem *mem, struct kefir_codegen_amd64 *codegen,
                                                struct kefir_codegen_amd64_sysv_module *module,
                                                kefir_ir_data_storage_t storage, kefir_bool_t defined,
                                                const char *section) {
    bool first = true;
    struct kefir_hashtree_node_iterator iter;
    const char *identifier = NULL;
    for (const struct kefir_ir_data *data = kefir_ir_module_named_data_iter(module->module, &iter, &identifier);
         data != NULL; data = kefir_ir_module_named_data_next(&iter, &identifier)) {
        if (data->storage != storage) {
            continue;
        }
        if (data->defined != defined) {
            continue;
        }

        if (first) {
            ASMGEN_SECTION(&codegen->asmgen, section);
            first = false;
        }
        REQUIRE_OK(kefir_amd64_sysv_static_data(mem, codegen, data, identifier));
    }
    return KEFIR_OK;
}

static kefir_result_t cg_translate_data(struct kefir_mem *mem, struct kefir_codegen_amd64 *codegen,
                                        struct kefir_codegen_amd64_sysv_module *module) {
    REQUIRE_OK(cg_translate_data_storage(mem, codegen, module, KEFIR_IR_DATA_GLOBAL_STORAGE, true, ".data"));
    REQUIRE_OK(cg_translate_data_storage(mem, codegen, module, KEFIR_IR_DATA_THREAD_LOCAL_STORAGE, true, ".tdata"));
    REQUIRE_OK(cg_translate_data_storage(mem, codegen, module, KEFIR_IR_DATA_GLOBAL_STORAGE, false, ".bss"));
    REQUIRE_OK(cg_translate_data_storage(mem, codegen, module, KEFIR_IR_DATA_THREAD_LOCAL_STORAGE, false, ".tbss"));
    REQUIRE_OK(cg_translate_strings(codegen, module));
    return KEFIR_OK;
}

static kefir_result_t cg_translate(struct kefir_mem *mem, struct kefir_codegen *cg_iface,
                                   struct kefir_ir_module *module) {
    REQUIRE(cg_iface != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid code generator interface"));
    REQUIRE(cg_iface->data != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 code generator"));
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg_iface->data;
    struct kefir_codegen_amd64_sysv_module sysv_module;
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_alloc(mem, &sysv_module, module));
    REQUIRE_OK(cg_module_prologue(&sysv_module, codegen));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(sysv_module.module, &iter); func != NULL;
         func = kefir_ir_module_function_next(&iter)) {
        REQUIRE_OK(cg_translate_function(mem, func, &sysv_module, codegen));
    }
    REQUIRE_OK(cg_translate_function_gates(codegen, &sysv_module.function_gates, false));
    REQUIRE_OK(cg_translate_function_gates(codegen, &sysv_module.function_vgates, true));
    REQUIRE_OK(cg_translate_tls_entries(codegen, &sysv_module.tls_entries, sysv_module.module));
    REQUIRE_OK(cg_translate_data(mem, codegen, &sysv_module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_free(mem, &sysv_module));
    return KEFIR_OK;
}

static kefir_result_t cg_close(struct kefir_codegen *cg) {
    REQUIRE(cg != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid code generator interface"));
    REQUIRE(cg->data != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 code generator"));
    struct kefir_codegen_amd64 *codegen = (struct kefir_codegen_amd64 *) cg->data;
    KEFIR_AMD64_ASMGEN_CLOSE(&codegen->asmgen);
    return KEFIR_OK;
}

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *codegen, FILE *out) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 code generator pointer"));
    REQUIRE_OK(kefir_amd64_gas_gen_init(&codegen->asmgen, out));
    codegen->iface.translate = cg_translate;
    codegen->iface.close = cg_close;
    codegen->iface.data = codegen;
    codegen->iface.self = codegen;
    codegen->config = &KefirCodegenDefaultConfiguration;
    return KEFIR_OK;
}
