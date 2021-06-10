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

struct result_return {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_amd64_sysv_function *func;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Unsupported function return type");
}

static kefir_result_t return_integer(const struct kefir_ir_type *type, kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct result_return *param = (struct result_return *) payload;
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_RAX);
    return KEFIR_OK;
}

static kefir_result_t return_float(const struct kefir_ir_type *type, kefir_size_t index,
                                   const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct result_return *param = (struct result_return *) payload;
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_INSTR(&param->codegen->asmgen, KEFIR_AMD64_PINSRQ);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_XMM0);
    ASMGEN_ARG0(&param->codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_ARG0(&param->codegen->asmgen, "0");
    return KEFIR_OK;
}

static kefir_result_t return_memory_aggregate(struct kefir_codegen_amd64 *codegen,
                                              struct kefir_amd64_sysv_data_layout *layout) {
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
    ASMGEN_ARG(&codegen->asmgen, KEFIR_SIZE_FMT, layout->size);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_CLD);
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_REP KEFIR_AMD64_MOVSB);
    return KEFIR_OK;
}

static kefir_result_t return_register_aggregate(struct kefir_codegen_amd64 *codegen,
                                                struct kefir_amd64_sysv_parameter_allocation *alloc) {
    kefir_size_t integer_register = 0;
    kefir_size_t sse_register = 0;
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_POP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    for (kefir_size_t i = 0; i < kefir_vector_length(&alloc->container.qwords); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&alloc->container.qwords, i));
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                if (integer_register >= KEFIR_AMD64_SYSV_INTEGER_RETURN_REGISTER_COUNT) {
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                           "Unable to return aggregate which exceeds available registers");
                }
                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_INTEGER_RETURN_REGISTERS[integer_register++]);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                           i * KEFIR_AMD64_SYSV_ABI_QWORD);
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                if (sse_register >= KEFIR_AMD64_SYSV_SSE_RETURN_REGISTER_COUNT) {
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                           "Unable to return aggregate which exceeds available registers");
                }
                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
                ASMGEN_ARG(&codegen->asmgen, KEFIR_AMD64_INDIRECT_OFFSET, KEFIR_AMD64_SYSV_ABI_DATA_REG,
                           i * KEFIR_AMD64_SYSV_ABI_QWORD);
                ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_PINSRQ);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_SSE_RETURN_REGISTERS[sse_register++]);
                ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_DATA2_REG);
                ASMGEN_ARG0(&codegen->asmgen, "0");
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED,
                                       "Return of non-integer,sse aggregate members is not supported");
        }
    }
    return KEFIR_OK;
}

static kefir_result_t return_aggregate(const struct kefir_ir_type *type, kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(typeentry);
    struct result_return *param = (struct result_return *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->func->decl.returns.allocation, iter.slot));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
                     kefir_vector_at(&param->func->decl.returns.layout, index));
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        REQUIRE_OK(return_memory_aggregate(param->codegen, layout));
    } else {
        REQUIRE_OK(return_register_aggregate(param->codegen, alloc));
    }
    return KEFIR_OK;
}

static kefir_result_t return_pad(const struct kefir_ir_type *type, kefir_size_t index,
                                 const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_OK;
}

static kefir_result_t return_builtin(const struct kefir_ir_type *type, kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry, void *payload) {
    struct result_return *param = (struct result_return *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
                     kefir_vector_at(&param->func->decl.returns.allocation, iter.slot));
    kefir_ir_builtin_type_t builtin = (kefir_ir_builtin_type_t) typeentry->param;
    REQUIRE(builtin < KEFIR_IR_TYPE_BUILTIN_COUNT, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unknown built-in type"));
    const struct kefir_codegen_amd64_sysv_builtin_type *builtin_type = KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[builtin];
    REQUIRE_OK(builtin_type->store_function_return(builtin_type, typeentry, param->codegen, alloc));
    return KEFIR_OK;
}

static kefir_result_t return_values(struct kefir_codegen_amd64 *codegen, const struct kefir_amd64_sysv_function *func) {
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, return_integer);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, return_float);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = return_aggregate;
    visitor.visit[KEFIR_IR_TYPE_UNION] = return_aggregate;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = return_aggregate;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = return_aggregate;
    visitor.visit[KEFIR_IR_TYPE_PAD] = return_pad;
    visitor.visit[KEFIR_IR_TYPE_BUILTIN] = return_builtin;
    struct result_return param = {.codegen = codegen, .func = func};
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(func->func->declaration->result, &visitor, (void *) &param, 0, 1));
    return KEFIR_OK;
}

static kefir_result_t restore_state(struct kefir_codegen_amd64 *codegen, const struct kefir_amd64_sysv_function *func) {
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_MOV);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG);
    if (func->frame.size > 0) {
        ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ADD);
        ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_RSP);
        ASMGEN_ARG(&codegen->asmgen, KEFIR_SIZE_FMT, func->frame.size);
    }
    ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_JMP);
    ASMGEN_ARG0(&codegen->asmgen, KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *codegen,
                                                  const struct kefir_amd64_sysv_function *func) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration"));
    ASMGEN_COMMENT(&codegen->asmgen, "Begin epilogue of %s", func->func->name);
    REQUIRE_OK(return_values(codegen, func));
    REQUIRE_OK(restore_state(codegen, func));
    ASMGEN_COMMENT(&codegen->asmgen, "End of %s", func->func->name);
    return KEFIR_OK;
}
