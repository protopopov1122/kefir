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


kefir_result_t vararg_classify_nested(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                    const struct kefir_ir_typeentry *typeentry,
                                    struct kefir_amd64_sysv_parameter_allocation *immediate_allocation,
                                    struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(immediate_allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid nested data classificator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid nested data classificator"));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_next(&immediate_allocation->container,
                                                KEFIR_AMD64_SYSV_PARAM_INTEGER,
                                                3 * KEFIR_AMD64_SYSV_ABI_QWORD,
                                                KEFIR_AMD64_SYSV_ABI_QWORD,
                                                &allocation->container_reference));
    return KEFIR_OK;
}

kefir_result_t vararg_classify_immediate(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                       const struct kefir_ir_typeentry *typeentry,
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

kefir_result_t vararg_allocate_immediate(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                       const struct kefir_ir_typeentry *typeentry,
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

kefir_result_t vararg_load_argument(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                  const struct kefir_ir_typeentry *typeentry,
                                  struct kefir_codegen_amd64 *codegen,
                                  struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_RBP,
        allocation->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PUSH);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

kefir_result_t vararg_return_value(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
                                 const struct kefir_ir_typeentry *typeentry,
                                 struct kefir_codegen_amd64 *codegen,
                                 struct kefir_amd64_sysv_parameter_allocation *allocation) {
    UNUSED(builtin_type);
    UNUSED(typeentry);
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid code generator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in data allocation"));
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET,
            KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
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
    ASMGEN_INSTR(&codegen->asmgen,
        KEFIR_AMD64_REP KEFIR_AMD64_MOVSB);
    return KEFIR_OK;
}

kefir_result_t vararg_store_argument(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
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
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        argument_index * KEFIR_AMD64_SYSV_ABI_QWORD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_LEA);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RDI);
    ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET,
        KEFIR_AMD64_RSP,
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

kefir_result_t vararg_load_return_value(const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type,
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

const struct kefir_codegen_amd64_sysv_builtin_type KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[] = {
    /* KEFIR_IR_TYPE_BUILTIN_VARARG */ {
        .layout = vararg_layout,
        .classify_nested = vararg_classify_nested,
        .classify_immediate = vararg_classify_immediate,
        .allocate_immediate = vararg_allocate_immediate,
        .load_argument = vararg_load_argument,
        .return_value = vararg_return_value,
        .store_argument = vararg_store_argument,
        .load_return_value = vararg_load_return_value
    }
};