#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/util.h"
#include <stdio.h>

static kefir_result_t preserve_state(struct kefir_amd64_asmgen *asmgen) {
    ASMGEN_INSTR1(asmgen, KEFIR_AMD64_CALL, KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE);
    return KEFIR_OK;
}

struct argument_load {
    const struct kefir_amd64_sysv_function *sysv_func;
    struct kefir_codegen_amd64 *codegen;
    kefir_size_t frame_offset;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Unsupported function parameter type");
}

static kefir_result_t load_integer_argument(const struct kefir_ir_type *type,
                                          kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry,
                                          void *payload) {
    UNUSED(typeentry);
    struct argument_load *param =
        (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    struct kefir_amd64_sysv_parameter_allocation *alloc = 
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(&param->sysv_func->parameters.allocation, iter.slot);
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_INTEGER) {
        ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_PUSH,
            KEFIR_AMD64_SYSV_INTEGER_REGISTERS[alloc->location.integer_register]);
    } else {
        ASMGEN_INSTR2(&param->codegen->asmgen, KEFIR_AMD64_MOV,
            KEFIR_AMD64_SYSV_ABI_DATA_REG,
            FORMAT(param->codegen->buf[0], "[rbp + %lu]", alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD));
        ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    }
    return KEFIR_OK;
}

static kefir_result_t load_sse_argument(const struct kefir_ir_type *type,
                                      kefir_size_t index,
                                      const struct kefir_ir_typeentry *typeentry,
                                      void *payload) {
    UNUSED(typeentry);
    struct argument_load *param =
        (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    struct kefir_amd64_sysv_parameter_allocation *alloc = 
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(&param->sysv_func->parameters.allocation, iter.slot);
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_SSE) {
        ASMGEN_INSTR3(&param->codegen->asmgen, KEFIR_AMD64_PEXTRQ,
            KEFIR_AMD64_SYSV_ABI_DATA_REG,
            KEFIR_AMD64_SYSV_SSE_REGISTERS[alloc->location.sse_register], "0");
    } else {
        ASMGEN_INSTR2(&param->codegen->asmgen, KEFIR_AMD64_MOV,
            KEFIR_AMD64_SYSV_ABI_DATA_REG,
            FORMAT(param->codegen->buf[0], "[rbp + %lu]", alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD));
    }
    ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

static kefir_result_t load_reg_aggregate(struct argument_load *param,
                                       struct kefir_amd64_sysv_data_layout *layout,
                                       struct kefir_amd64_sysv_parameter_allocation *alloc) {
    param->frame_offset = kefir_codegen_pad_aligned(param->frame_offset, layout->alignment);
    for (kefir_size_t i = 0; i < kefir_vector_length(&alloc->container.qwords); i++) {
        struct kefir_amd64_sysv_abi_qword *qword =
            (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&alloc->container.qwords, i);
        switch (qword->klass) {
            case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                ASMGEN_INSTR2(&param->codegen->asmgen, KEFIR_AMD64_MOV,
                    FORMAT(param->codegen->buf[0], "[%s + %lu]",
                        KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                        param->frame_offset + KEFIR_AMD64_SYSV_ABI_QWORD * i),
                    KEFIR_AMD64_SYSV_INTEGER_REGISTERS[qword->location]);
                break;

            case KEFIR_AMD64_SYSV_PARAM_SSE:
                ASMGEN_INSTR3(&param->codegen->asmgen, KEFIR_AMD64_PEXTRQ,
                    KEFIR_AMD64_SYSV_ABI_DATA_REG,
                    KEFIR_AMD64_SYSV_SSE_REGISTERS[qword->location], "0");
                ASMGEN_INSTR2(&param->codegen->asmgen, KEFIR_AMD64_MOV,
                    FORMAT(param->codegen->buf[0], "[%s + %lu]",
                        KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                        param->frame_offset + KEFIR_AMD64_SYSV_ABI_QWORD * i),
                    KEFIR_AMD64_SYSV_ABI_DATA_REG);
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Aggregates with non-INTEGER and non-SSE members are not supported yet");
        }
    }
    ASMGEN_INSTR2(&param->codegen->asmgen, KEFIR_AMD64_LEA, KEFIR_AMD64_SYSV_ABI_DATA_REG,
        FORMAT(param->codegen->buf[0], "[%s + %lu]",
            KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
            param->frame_offset));
    param->frame_offset += layout->size;
    return KEFIR_OK;
}

static kefir_result_t load_aggregate_argument(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(typeentry);
    struct argument_load *param =
        (struct argument_load *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    struct kefir_amd64_sysv_parameter_allocation *alloc = 
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(&param->sysv_func->parameters.allocation, iter.slot);
    if (alloc->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        ASMGEN_INSTR2(&param->codegen->asmgen, KEFIR_AMD64_LEA,
            KEFIR_AMD64_SYSV_ABI_DATA_REG,
            FORMAT(param->codegen->buf[0], "[rbp + %lu]", alloc->location.stack_offset + 2 * KEFIR_AMD64_SYSV_ABI_QWORD));
    } else {
        struct kefir_amd64_sysv_data_layout *layout = 
            (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(&param->sysv_func->parameters.layout, index);
        REQUIRE_OK(load_reg_aggregate(param, layout, alloc));
    }
    ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    return KEFIR_OK;
}

static kefir_result_t load_pad_argument(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_OK;
}

static kefir_result_t load_arguments(struct kefir_codegen_amd64 *codegen,
                                   const struct kefir_amd64_sysv_function *sysv_func) {
    if (sysv_func->frame.size > 0) {
        ASMGEN_INSTR2(&codegen->asmgen, KEFIR_AMD64_ADD, KEFIR_AMD64_RSP,
            FORMAT(codegen->buf[0], "%lu", sysv_func->frame.size));
    }
    ASMGEN_INSTR2(&codegen->asmgen, KEFIR_AMD64_MOV, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG, KEFIR_AMD64_RSP);
    const struct kefir_irfunction_decl *func = &sysv_func->func->declaration;
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Load parameters of %s", func->identifier));
    if (sysv_func->internals[KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS].enabled) {
        ASMGEN_INSTR2(&codegen->asmgen, KEFIR_AMD64_MOV,
            FORMAT(codegen->buf[0], "[%s + %lu]",
                KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG,
                sysv_func->internals[KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS].offset),
            KEFIR_AMD64_RDI);
    }
    struct argument_load param = {
        .sysv_func = sysv_func,
        .codegen = codegen,
        .frame_offset = sysv_func->frame.base.parameters
    };
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, load_integer_argument);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, load_sse_argument);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_UNION] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = load_aggregate_argument;
    visitor.visit[KEFIR_IR_TYPE_PAD] = load_pad_argument;
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(&func->params, &visitor,
        (void *) &param, 0, kefir_ir_type_nodes(&func->params)));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *codegen,
                                              const struct kefir_amd64_sysv_function *func) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration"));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Begin prologue of %s", func->func->declaration.identifier));
    REQUIRE_OK(preserve_state(&codegen->asmgen));
    REQUIRE_OK(load_arguments(codegen, func));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "End prologue of %s", func->func->declaration.identifier));
    return KEFIR_OK;
}