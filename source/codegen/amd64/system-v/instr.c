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

#include "kefir/codegen/amd64/system-v/instr.h"
#include "kefir/codegen/amd64/system-v/abi/vararg.h"
#include "kefir/codegen/amd64/labels.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/opcodes.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/core/error.h"

static kefir_result_t cg_symbolic_opcode(kefir_iropcode_t opcode, const char **symbolic) {
    REQUIRE(symbolic != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to symbolic opcode"));
    *symbolic = kefir_amd64_iropcode_handler(opcode);
    REQUIRE(*symbolic != NULL, KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to find symbolic opcode representation"));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_instruction(struct kefir_mem *mem, struct kefir_codegen_amd64 *codegen,
                                            struct kefir_amd64_sysv_function *sysv_func,
                                            struct kefir_codegen_amd64_sysv_module *sysv_module,
                                            const struct kefir_irinstr *instr) {
    switch (instr->opcode) {
        case KEFIR_IROPCODE_JMP:
        case KEFIR_IROPCODE_BRANCH: {
            const char *opcode_symbol = NULL;
            REQUIRE(instr->arg.u64 <= kefir_irblock_length(&sysv_func->func->body),
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Jump offset is out of IR block bounds"));
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_BODY_LABEL " + " KEFIR_INT64_FMT,
                       sysv_func->func->name, 2 * KEFIR_AMD64_SYSV_ABI_QWORD * instr->arg.u64);
        } break;

        case KEFIR_IROPCODE_RET: {
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_PROCEDURE_EPILOGUE_LABEL, sysv_func->func->name);
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_INVOKE: {
            kefir_id_t id = (kefir_id_t) instr->arg.u64;
            REQUIRE(kefir_codegen_amd64_sysv_module_function_decl(mem, sysv_module, id, false),
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE,
                                    "Failed to allocate AMD64 System-V IR module function decaration"));

            const struct kefir_ir_function_decl *decl = kefir_ir_module_get_declaration(sysv_module->module, id);
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            if (decl->name == NULL || decl->vararg) {
                ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_GATE_ID_LABEL, id);
            } else {
                ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_GATE_NAMED_LABEL, decl->name, decl->id);
            }
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_INVOKEV: {
            kefir_id_t id = (kefir_id_t) instr->arg.u64;
            REQUIRE(kefir_codegen_amd64_sysv_module_function_decl(mem, sysv_module, id, true),
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE,
                                    "Failed to allocate AMD64 System-V IR module function decaration"));

            const struct kefir_ir_function_decl *decl = kefir_ir_module_get_declaration(sysv_module->module, id);
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            if (decl->name == NULL) {
                ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_ID_LABEL, id);
            } else {
                ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSV_FUNCTION_VIRTUAL_GATE_NAMED_LABEL, decl->name);
            }
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_PUSHSTRING: {
            const kefir_id_t identifier = (kefir_id_t) instr->arg.u64;

            kefir_ir_string_literal_type_t type;
            kefir_bool_t public;
            const void *content;
            kefir_size_t length;
            REQUIRE_OK(
                kefir_ir_module_get_string_literal(sysv_module->module, identifier, &type, &public, &content, &length));
            REQUIRE(public, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Cannot push non-public string literal"));

            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_PUSHI64, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_STRING_LITERAL, identifier);
        } break;

        case KEFIR_IROPCODE_BZERO:
        case KEFIR_IROPCODE_BCOPY: {
            const kefir_id_t type_id = (kefir_id_t) instr->arg.u32[0];
            const kefir_size_t type_index = (kefir_size_t) instr->arg.u32[1];
            struct kefir_vector *layout = kefir_codegen_amd64_sysv_module_type_layout(mem, sysv_module, type_id);
            REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unknown named type"));
            ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, entry, kefir_vector_at(layout, type_index));
            REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to retrieve type node at index"));

            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, entry->size);
        } break;

        case KEFIR_IROPCODE_EXTUBITS:
        case KEFIR_IROPCODE_EXTSBITS:
        case KEFIR_IROPCODE_INSERTBITS: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_DOUBLE);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_UINT32_FMT, instr->arg.u32[0]);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_UINT32_FMT, instr->arg.u32[1]);
        } break;

        case KEFIR_IROPCODE_OFFSETPTR:
        case KEFIR_IROPCODE_ELEMENTPTR: {
            const kefir_id_t type_id = (kefir_id_t) instr->arg.u32[0];
            const kefir_size_t type_index = (kefir_size_t) instr->arg.u32[1];
            struct kefir_vector *layout = kefir_codegen_amd64_sysv_module_type_layout(mem, sysv_module, type_id);
            REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unknown named type"));
            ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, entry, kefir_vector_at(layout, type_index));
            REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to retrieve type node at index"));
            const char *opcode_symbol = NULL;
            if (instr->opcode == KEFIR_IROPCODE_OFFSETPTR) {
                REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_OFFSETPTR, &opcode_symbol));
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
                ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, entry->relative_offset);
            } else {
                REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_ELEMENTPTR, &opcode_symbol));
                ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
                ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, entry->size);
            }
        } break;

        case KEFIR_IROPCODE_GETLOCALS: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, sysv_func->frame.base.locals);
        } break;

        case KEFIR_IROPCODE_GETGLOBAL: {
            const kefir_id_t named_symbol = (kefir_id_t) instr->arg.u64;
            const char *symbol = kefir_ir_module_get_named_symbol(sysv_module->module, named_symbol);
            REQUIRE(symbol != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to find named symbol"));

            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_PUSHI64, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG0(&codegen->asmgen, symbol);
        } break;

        case KEFIR_IROPCODE_VARARG_START:
        case KEFIR_IROPCODE_VARARG_COPY:
        case KEFIR_IROPCODE_VARARG_GET:
        case KEFIR_IROPCODE_VARARG_END:
            return kefir_codegen_amd64_sysv_vararg_instruction(mem, codegen, sysv_module, sysv_func, instr);

        case KEFIR_IROPCODE_PUSHF32: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_PUSHI64, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_DOUBLE);
            ASMGEN_ARG(&codegen->asmgen, "%a", instr->arg.f32[0]);
            ASMGEN_ARG0(&codegen->asmgen, "0");
        } break;

        case KEFIR_IROPCODE_PUSHF64: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_PUSHI64, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, "%a", instr->arg.f64);
        } break;

        case KEFIR_IROPCODE_PUSHU64: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(KEFIR_IROPCODE_PUSHI64, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_UINT64_FMT, instr->arg.u64);
        } break;

        default: {
            const char *opcode_symbol = NULL;
            REQUIRE_OK(cg_symbolic_opcode(instr->opcode, &opcode_symbol));
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
            ASMGEN_ARG0(&codegen->asmgen, opcode_symbol);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, instr->arg);
        } break;
    }
    return KEFIR_OK;
}
