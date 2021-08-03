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

#include <string.h>
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi/builtins.h"

struct invoke_info {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_amd64_sysv_function_decl *decl;
    const kefir_size_t total_arguments;
    kefir_size_t argument;
};

struct invoke_returns {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_amd64_sysv_function_decl *decl;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX
                           "Encountered not supported type code while traversing type");
}

static kefir_result_t scalar_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                      const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, allocation,
                     kefir_vector_at(&info->decl->parameters.allocation, iter.slot));
    switch (allocation->klass) {
        case KEFIR_AMD64_SYSV_PARAM_INTEGER:
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&info->codegen->asmgen,
                        KEFIR_AMD64_SYSV_INTEGER_REGISTERS[allocation->location.integer_register]);
            ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                       (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
            break;

        case KEFIR_AMD64_SYSV_PARAM_SSE:
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PINSRQ);
            ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_SSE_REGISTERS[allocation->location.sse_register]);
            ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                       (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
            ASMGEN_ARG0(&info->codegen->asmgen, "0");
            break;

        case KEFIR_AMD64_SYSV_PARAM_MEMORY:
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
            ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                       (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
            ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RSP,
                       allocation->location.stack_offset);
            ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                   "Integer function argument cannot have non-INTEGER and non-MEMORY class");
    }
    info->argument++;
    return KEFIR_OK;
}

static kefir_result_t memory_aggregate_argument(struct invoke_info *info, struct kefir_amd64_sysv_data_layout *layout,
                                                struct kefir_amd64_sysv_parameter_allocation *allocation) {
    if (layout->size > 0) {
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RDI);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RSI);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RCX);

        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RCX);
        ASMGEN_ARG(&info->codegen->asmgen, KEFIR_INT64_FMT, layout->size);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RSI);
        ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                   (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_LEA);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RDI);
        ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RSP,
                   3 * KEFIR_AMD64_SYSV_ABI_QWORD + allocation->location.stack_offset);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_CLD);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_REP KEFIR_AMD64_MOVSB);

        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_POP);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RCX);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_POP);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RSI);
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_POP);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RDI);
    }
    return KEFIR_OK;
}

static kefir_result_t register_aggregate_argument(struct invoke_info *info,
                                                  struct kefir_amd64_sysv_parameter_allocation *allocation) {
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
               (info->total_arguments - info->argument - 1) * KEFIR_AMD64_SYSV_ABI_QWORD);
    for (kefir_size_t i = 0; i < kefir_vector_length(&allocation->container.qwords); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&allocation->container.qwords, i));
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_INTEGER_REGISTERS[qword->location]);
                ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_TMP_REG,
                           i * KEFIR_AMD64_SYSV_ABI_QWORD);
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PINSRQ);
                ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_SSE_REGISTERS[qword->location]);
                ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_TMP_REG,
                           i * KEFIR_AMD64_SYSV_ABI_QWORD);
                ASMGEN_ARG0(&info->codegen->asmgen, "0");
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Non-INTEGER & non-SSE arguments are not supported");
        }
    }
    return KEFIR_OK;
}

static kefir_result_t aggregate_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                         const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, allocation,
                     kefir_vector_at(&info->decl->parameters.allocation, iter.slot));
    if (allocation->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
                         kefir_vector_at(&info->decl->parameters.layout, index));
        REQUIRE_OK(memory_aggregate_argument(info, layout, allocation));
    } else {
        REQUIRE_OK(register_aggregate_argument(info, allocation));
    }
    info->argument++;
    return KEFIR_OK;
}

static kefir_result_t visit_pad(const struct kefir_ir_type *type, kefir_size_t index,
                                const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_OK;
}

static kefir_result_t builtin_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, allocation,
                     kefir_vector_at(&info->decl->parameters.allocation, iter.slot));
    kefir_ir_builtin_type_t builtin = (kefir_ir_builtin_type_t) typeentry->param;
    REQUIRE(builtin < KEFIR_IR_TYPE_BUILTIN_COUNT, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unknown built-in type"));
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type = KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[builtin];
    REQUIRE_OK(builtin_type->store_function_argument(builtin_type, typeentry, info->codegen, allocation,
                                                     info->total_arguments - info->argument - 1));
    info->argument++;
    return KEFIR_OK;
}

kefir_result_t invoke_prologue(struct kefir_codegen_amd64 *codegen, const struct kefir_amd64_sysv_function_decl *decl,
                               struct invoke_info *info) {
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, scalar_argument);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, scalar_argument);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_UNION] = aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_PAD] = visit_pad;
    visitor.visit[KEFIR_IR_TYPE_BUILTIN] = builtin_argument;

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    if (decl->parameters.location.stack_offset > 0) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_SUB);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, decl->parameters.location.stack_offset);
    }
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_AND);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, ~0xfll);
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(decl->decl->params, &visitor, (void *) info, 0, info->total_arguments));
    if (decl->returns.implicit_parameter) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_INTEGER_REGISTERS[0]);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                   KEFIR_AMD64_SYSV_INTERNAL_BOUND);
    }
    if (decl->decl->vararg) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, decl->parameters.location.sse_register);
    }
    return KEFIR_OK;
}

static kefir_result_t integer_return(const struct kefir_ir_type *type, kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RAX);
    return KEFIR_OK;
}

static kefir_result_t sse_return(const struct kefir_ir_type *type, kefir_size_t index,
                                 const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PEXTRQ);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_XMM0);
    ASMGEN_ARG0(&info->codegen->asmgen, "0");
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    return KEFIR_OK;
}

static kefir_result_t register_aggregate_return(struct invoke_info *info,
                                                struct kefir_amd64_sysv_parameter_allocation *allocation) {
    kefir_size_t integer_register = 0;
    kefir_size_t sse_register = 0;
    for (kefir_size_t i = 0; i < kefir_vector_length(&allocation->container.qwords); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&allocation->container.qwords, i));
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                if (integer_register >= KEFIR_AMD64_SYSV_INTEGER_RETURN_REGISTER_COUNT) {
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                           "Unable to return aggregate which exceeds available registers");
                }
                ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                           KEFIR_AMD64_SYSV_INTERNAL_BOUND + i * KEFIR_AMD64_SYSV_ABI_QWORD);
                ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_INTEGER_RETURN_REGISTERS[integer_register++]);
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                if (sse_register >= KEFIR_AMD64_SYSV_SSE_RETURN_REGISTER_COUNT) {
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                           "Unable to return aggregate which exceeds available registers");
                }
                ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PEXTRQ);
                ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                           KEFIR_AMD64_SYSV_INTERNAL_BOUND + i * KEFIR_AMD64_SYSV_ABI_QWORD);
                ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_SSE_RETURN_REGISTERS[sse_register++]);
                ASMGEN_ARG0(&info->codegen->asmgen, "0");
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Non-INTEGER & non-SSE arguments are not supported");
        }
    }
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    ASMGEN_ARG(&info->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
               KEFIR_AMD64_SYSV_INTERNAL_BOUND);
    ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_TMP_REG);
    return KEFIR_OK;
}

static kefir_result_t aggregate_return(const struct kefir_ir_type *type, kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, allocation,
                     kefir_vector_at(&info->decl->returns.allocation, iter.slot));
    if (allocation->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        ASMGEN_INSTR(&info->codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&info->codegen->asmgen, KEFIR_AMD64_RAX);
    } else {
        REQUIRE_OK(register_aggregate_return(info, allocation));
    }
    return KEFIR_OK;
}

static kefir_result_t builtin_return(const struct kefir_ir_type *type, kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry, void *payload) {
    ASSIGN_DECL_CAST(struct invoke_info *, info, payload);
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, allocation,
                     kefir_vector_at(&info->decl->returns.allocation, iter.slot));
    kefir_ir_builtin_type_t builtin = (kefir_ir_builtin_type_t) typeentry->param;
    REQUIRE(builtin < KEFIR_IR_TYPE_BUILTIN_COUNT, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unknown built-in type"));
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type = KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[builtin];
    REQUIRE_OK(builtin_type->load_function_return(builtin_type, typeentry, info->codegen, allocation));
    return KEFIR_OK;
}

kefir_result_t invoke_epilogue(struct kefir_codegen_amd64 *codegen, const struct kefir_amd64_sysv_function_decl *decl,
                               struct invoke_info *info, bool virtualDecl) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    if (info->total_arguments > 0 || virtualDecl) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT,
                   (info->total_arguments + (virtualDecl ? 1 : 0)) * KEFIR_AMD64_SYSV_ABI_QWORD);
    }
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, integer_return);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, sse_return);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = aggregate_return;
    visitor.visit[KEFIR_IR_TYPE_UNION] = aggregate_return;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = aggregate_return;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = aggregate_return;
    visitor.visit[KEFIR_IR_TYPE_PAD] = visit_pad;
    visitor.visit[KEFIR_IR_TYPE_BUILTIN] = builtin_return;
    REQUIRE(kefir_ir_type_nodes(decl->decl->result) <= 1,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function cannot return more than one value"));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(decl->decl->result, &visitor, (void *) info, 0, 1));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_invoke(struct kefir_codegen_amd64 *codegen,
                                                const struct kefir_amd64_sysv_function_decl *decl, bool virtualDecl) {
    struct invoke_info info = {
        .codegen = codegen, .decl = decl, .total_arguments = kefir_ir_type_nodes(decl->decl->params), .argument = 0};
    REQUIRE_OK(invoke_prologue(codegen, decl, &info));
    if (virtualDecl) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                   info.total_arguments * KEFIR_AMD64_SYSV_ABI_QWORD);
    } else {
        REQUIRE(decl->decl->name != NULL && strlen(decl->decl->name) != 0,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to translate invocation with no valid identifier"));
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
        ASMGEN_ARG0(&codegen->asmgen, decl->decl->name);
    }
    REQUIRE_OK(invoke_epilogue(codegen, decl, &info, virtualDecl));
    return KEFIR_OK;
}
