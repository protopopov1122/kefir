/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/codegen/amd64/system-v/abi/builtins.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/util.h"
#include "kefir/core/error.h"

static kefir_result_t vararg_layout(const struct kefir_codegen_amd64_sysv_builtin_type *builin_type,
                                    const struct kefir_ir_typeentry *typeentry,
                                    struct kefir_amd64_sysv_data_layout *data_layout) {
    UNUSED(builin_type);
    UNUSED(typeentry);
    REQUIRE(data_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid data layout pointer"));
    data_layout->aligned = true;
    data_layout->size = 3 * KEFIR_AMD64_SYSV_ABI_QWORD;
    data_layout->alignment = KEFIR_AMD64_SYSV_ABI_QWORD;
    return KEFIR_OK;
}

static kefir_result_t vararg_classify_nested_argument(
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type, const struct kefir_ir_typeentry *typeentry,
    struct kefir_amd64_sysv_parameter_allocation *immediate_allocation,
    struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(immediate_allocation != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid nested data classificator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid nested data classificator"));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_next(&immediate_allocation->container, KEFIR_AMD64_SYSV_PARAM_INTEGER,
                                                3 * KEFIR_AMD64_SYSV_ABI_QWORD, KEFIR_AMD64_SYSV_ABI_QWORD,
                                                &allocation->container_reference));
    return KEFIR_OK;
}

static kefir_result_t vararg_classify_immediate_argument(
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type, const struct kefir_ir_typeentry *typeentry,
    struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid nested data classificator"));
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    allocation->requirements.memory.size = 3 * KEFIR_AMD64_SYSV_ABI_QWORD;
    allocation->requirements.memory.alignment = KEFIR_AMD64_SYSV_ABI_QWORD;
    return KEFIR_OK;
}

static kefir_result_t vararg_allocate_immediate_argument(
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type, const struct kefir_ir_typeentry *typeentry,
    struct kefir_amd64_sysv_parameter_location *total_allocation,
    struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(total_allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    total_allocation->stack_offset =
        kefir_codegen_pad_aligned(total_allocation->stack_offset, KEFIR_AMD64_SYSV_ABI_QWORD);
    allocation->location.stack_offset = total_allocation->stack_offset;
    total_allocation->stack_offset += 3 * KEFIR_AMD64_SYSV_ABI_QWORD;
    return KEFIR_OK;
}

static kefir_result_t vararg_load_function_argument(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                                    const struct kefir_ir_typeentry *typeentry,
                                                    struct kefir_codegen_amd64 *codegen,
                                                    struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RBP,
               allocation->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

static kefir_result_t vararg_store_function_return(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                                   const struct kefir_ir_typeentry *typeentry,
                                                   struct kefir_codegen_amd64 *codegen,
                                                   struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
               KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RCX);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_SIZE_FMT, 3 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CLD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_REP KEFIR_AMD64_MOVSB);
    return KEFIR_OK;
}

static kefir_result_t vararg_store_function_argument(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                                     const struct kefir_ir_typeentry *typeentry,
                                                     struct kefir_codegen_amd64 *codegen,
                                                     struct kefir_amd64_sysv_parameter_allocation *allocation,
                                                     kefir_size_t argument_index) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RCX);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RCX);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 3 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
               argument_index * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_RSP,
               3 * KEFIR_AMD64_SYSV_ABI_QWORD + allocation->location.stack_offset);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CLD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_REP KEFIR_AMD64_MOVSB);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RCX);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSI);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    return KEFIR_OK;
}

static kefir_result_t vararg_load_function_return(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                                  const struct kefir_ir_typeentry *typeentry,
                                                  struct kefir_codegen_amd64 *codegen,
                                                  struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RAX);
    return KEFIR_OK;
}

static kefir_result_t vararg_load_vararg_impl(struct kefir_codegen_amd64 *codegen) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
               KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 3 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
               KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
    return KEFIR_OK;
}

static kefir_result_t vararg_load_vararg_appendix(struct kefir_codegen_amd64 *codegen,
                                                  struct kefir_codegen_amd64_sysv_module *sysv_module,
                                                  const struct kefir_amd64_sysv_function *sysv_func,
                                                  const char *identifier, void *payload) {
    UNUSED(sysv_module);
    UNUSED(sysv_func);
    UNUSED(payload);
    ASMGEN_LABEL(&codegen->asmgen, "%s", identifier);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);

    REQUIRE_OK(vararg_load_vararg_impl(codegen));

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RBX);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_INT64_FMT, 2 * KEFIR_AMD64_SYSV_ABI_QWORD);

    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT, KEFIR_AMD64_RBX);
    return KEFIR_OK;
}

static kefir_result_t vararg_load_vararg(struct kefir_mem *mem,
                                         const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                         const struct kefir_ir_typeentry *typeentry,
                                         struct kefir_codegen_amd64 *codegen,
                                         struct kefir_amd64_sysv_function *sysv_func, const char *identifier,
                                         struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(sysv_func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 System-V function"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    kefir_result_t res =
        kefir_amd64_sysv_function_insert_appendix(mem, sysv_func, vararg_load_vararg_appendix, NULL, NULL, identifier);
    REQUIRE(res == KEFIR_OK || res == KEFIR_ALREADY_EXISTS, res);
    kefir_result_t result = res;
    ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_QUAD);
    ASMGEN_ARG0(&codegen->asmgen, identifier);
    ASMGEN_ARG0(&codegen->asmgen, "0");
    return result;
    return KEFIR_OK;
}

const struct kefir_codegen_amd64_sysv_builtin_type KEFIR_CODEGEN_AMD64_SYSV_BUILIN_VARARG_TYPE = {
    .layout = vararg_layout,
    .classify_nested_argument = vararg_classify_nested_argument,
    .classify_immediate_argument = vararg_classify_immediate_argument,
    .allocate_immediate_argument = vararg_allocate_immediate_argument,
    .load_function_argument = vararg_load_function_argument,
    .store_function_return = vararg_store_function_return,
    .store_function_argument = vararg_store_function_argument,
    .load_function_return = vararg_load_function_return,
    .load_vararg = vararg_load_vararg};
