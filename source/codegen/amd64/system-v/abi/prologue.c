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

#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/amd64/system-v/abi/builtins.h"
#include "kefir/codegen/util.h"
#include <stdio.h>

static kefir_result_t preserve_state(struct kefir_amd64_asmgen *asmgen) {
    ASMGEN_INSTR(asmgen, KEFIR_AMD64_CALL);
    ASMGEN_ARG0(asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE);
    return KEFIR_OK;
}

struct argument_load {
    const struct kefir_amd64_sysv_function *sysv_func;
    struct kefir_codegen_amd64 *codegen;
    kefir_size_t frame_offset;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Unsupported function parameter type");
}

static kefir_result_t mask_argument(struct kefir_codegen_amd64 *codegen, const struct kefir_ir_typeentry *typeentry,
                                    const char *reg) {
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_EAX);
            ASMGEN_ARG0(&codegen->asmgen, "0xff");

            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_AND);
            ASMGEN_ARG0(&codegen->asmgen, reg);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_EAX);
            ASMGEN_ARG0(&codegen->asmgen, "0xffff");

            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_AND);
            ASMGEN_ARG0(&codegen->asmgen, reg);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_FLOAT32:
        case KEFIR_IR_TYPE_INT32:
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_EAX);
            ASMGEN_ARG0(&codegen->asmgen, "-1");

            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_AND);
            ASMGEN_ARG0(&codegen->asmgen, reg);
            ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
            break;

        case KEFIR_IR_TYPE_INT64:
        case KEFIR_IR_TYPE_FLOAT64:
        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
            // Do nothing
            break;

        case KEFIR_IR_TYPE_BITS: {
            kefir_size_t bits = typeentry->param;
            if (bits > 0) {
                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_SHL);
                ASMGEN_ARG0(&codegen->asmgen, reg);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 64 - bits);

                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_SHR);
                ASMGEN_ARG0(&codegen->asmgen, reg);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 64 - bits);
            }
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected argument type");
    }
    return KEFIR_OK;
}

static kefir_result_t load_integer_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    struct argument_load *param = (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->sysv_func->decl.parameters.allocation, iter.slot));
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_INTEGER) {
        ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_INTEGER_REGISTERS[alloc->location.integer_register]);
    } else {
        ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_MOV);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
        ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RBP,
                   alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
        REQUIRE_OK(mask_argument(param->codegen, typeentry, KEFIR_AMD64_SYSV_ABI_DATA_REG));
        ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PUSH);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    }
    return KEFIR_OK;
}

static kefir_result_t load_sse_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    struct argument_load *param = (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->sysv_func->decl.parameters.allocation, iter.slot));
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_SSE) {
        ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PEXTRQ);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_SSE_REGISTERS[alloc->location.sse_register]);
        ASMGEN_ARG0(&param->codegen->asmgen, "0");
    } else {
        ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_MOV);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
        ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RBP,
                   alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
        REQUIRE_OK(mask_argument(param->codegen, typeentry, KEFIR_AMD64_SYSV_ABI_DATA_REG));
    }
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

static kefir_result_t load_long_double_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                                const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    struct argument_load *param = (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->sysv_func->decl.parameters.allocation, iter.slot));
    REQUIRE(alloc->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected long double argument to be allocated in memory"));

    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RBP,
               alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD + KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);

    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RBP,
               alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

static kefir_result_t load_reg_aggregate(struct argument_load *param, struct kefir_amd64_sysv_data_layout *layout,
                                         struct kefir_amd64_sysv_parameter_allocation *alloc) {
    param->frame_offset = kefir_codegen_pad_aligned(param->frame_offset, layout->alignment);
    for (kefir_size_t i = 0; i < kefir_vector_length(&alloc->container.qwords); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&alloc->container.qwords, i));
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                           param->frame_offset + KEFIR_AMD64_SYSV_ABI_QWORD * i);
                ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_INTEGER_REGISTERS[qword->location]);
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PEXTRQ);
                ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
                ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_SSE_REGISTERS[qword->location]);
                ASMGEN_ARG0(&param->codegen->asmgen, "0");
                ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                           param->frame_offset + KEFIR_AMD64_SYSV_ABI_QWORD * i);
                ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                       "Aggregates with non-INTEGER and non-SSE members are not supported yet");
        }
    }
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
               param->frame_offset);
    param->frame_offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t load_aggregate_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                              const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    struct argument_load *param = (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->sysv_func->decl.parameters.allocation, iter.slot));
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_LEA);
        ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
        ASMGEN_ARG(&param->codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RBP,
                   alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    } else {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
                         kefir_vector_at(&param->sysv_func->decl.parameters.layout, index));
        REQUIRE_OK(load_reg_aggregate(param, layout, alloc));
    }
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

static kefir_result_t load_pad_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_OK;
}

static kefir_result_t load_builtin_argument(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    struct argument_load *param = (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->sysv_func->decl.parameters.allocation, iter.slot));
    kefir_ir_builtin_type_t builtin = (kefir_ir_builtin_type_t) typeentry->param;
    REQUIRE(builtin < KEFIR_IR_TYPE_BUILTIN_COUNT, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unknown built-in type"));
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type = KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[builtin];
    REQUIRE_OK(builtin_type->load_function_argument(builtin_type, typeentry, param->codegen, alloc));
    return KEFIR_OK;
}

static kefir_result_t load_arguments(struct kefir_codegen_amd64 *codegen,
                                     const struct kefir_amd64_sysv_function *sysv_func) {
    if (sysv_func->frame.size > 0) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_SUB);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_SIZE_FMT, sysv_func->frame.size);
    }
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_GENERIC_PROLOGUE);
    const struct kefir_ir_function_decl *func = sysv_func->func->declaration;
    ASMGEN_COMMENT(&codegen->asmgen, "Load parameters of %s", func->name);
    if (sysv_func->decl.returns.implicit_parameter) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                   KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS * KEFIR_AMD64_SYSV_ABI_QWORD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    }
    struct argument_load param = {
        .sysv_func = sysv_func, .codegen = codegen, .frame_offset = sysv_func->frame.base.parameters};
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, load_integer_argument);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, load_sse_argument);
    visitor.visit[KEFIR_IR_TYPE_LONG_DOUBLE] = load_long_double_argument;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_UNION] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_PAD] = load_pad_argument;
    visitor.visit[KEFIR_IR_TYPE_BUILTIN] = load_builtin_argument;
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(func->params, &visitor, (void *) &param, 0,
                                                kefir_ir_type_nodes(func->params)));
    return KEFIR_OK;
}

static kefir_result_t save_registers(struct kefir_codegen_amd64 *codegen,
                                     const struct kefir_amd64_sysv_function *sysv_func) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
               sysv_func->frame.base.register_save_area);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CALL);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_SAVE_REGISTERS);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *codegen,
                                                  const struct kefir_amd64_sysv_function *func) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 code generator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function declaration"));
    ASMGEN_COMMENT(&codegen->asmgen, "Begin prologue of %s", func->func->name);
    REQUIRE_OK(preserve_state(&codegen->asmgen));
    REQUIRE_OK(load_arguments(codegen, func));
    if (func->func->declaration->vararg) {
        REQUIRE_OK(save_registers(codegen, func));
    }
    ASMGEN_COMMENT(&codegen->asmgen, "End prologue of %s", func->func->name);
    return KEFIR_OK;
}
