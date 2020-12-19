#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"
#include "kefir/codegen/amd64/system-v/abi_allocation.h"
#include <stdio.h>

static kefir_result_t preserve_state(struct kefir_amd64_asmgen *asmgen) {
    ASMGEN_INSTR1(asmgen, KEFIR_AMD64_CALL, KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE);
    return KEFIR_OK;
}

static kefir_result_t restore_state(struct kefir_amd64_asmgen *asmgen) {
    ASMGEN_INSTR1(asmgen, KEFIR_AMD64_JMP, KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE);
    return KEFIR_OK;
}

struct argument_load {
    struct kefir_vector *allocation;
    struct kefir_codegen_amd64 *codegen;
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
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(param->allocation, iter.slot);
    ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_SYSV_INTEGER_REGISTERS[alloc->location.integer_register]);
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
        (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(param->allocation, iter.slot);
    ASMGEN_INSTR3(&param->codegen->asmgen, KEFIR_AMD64_PEXTRQ, KEFIR_AMD64_R12, KEFIR_AMD64_SYSV_SSE_REGISTERS[alloc->location.sse_register], "0");
    ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_R12);
    return KEFIR_OK;
}

static kefir_result_t load_arguments(struct kefir_codegen_amd64 *codegen,
                                   const struct kefir_irfunction_decl *func) {
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Load parameters of %s", func->identifier));
    struct kefir_vector layout, allocation;
    REQUIRE_OK(kefir_amd64_sysv_type_layout(&func->params, codegen->mem, &layout));
    kefir_result_t res = kefir_amd64_sysv_parameter_classify(codegen->mem, &func->params, &layout, &allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(codegen->mem, &layout);
        return res;
    });
    struct kefir_amd64_sysv_parameter_location location = {0};
    res = kefir_amd64_sysv_parameter_allocate(codegen->mem, &func->params, &layout, &allocation, &location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(codegen->mem, &allocation);
        kefir_vector_free(codegen->mem, &layout);
        return res;
    });
    struct argument_load param = {
        .allocation = &allocation,
        .codegen = codegen
    };
    struct kefir_ir_type_visitor visitor;
    res = kefir_ir_type_visitor_init(&visitor, visitor_not_supported);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(codegen->mem, &allocation);
        kefir_vector_free(codegen->mem, &layout);
        return res;
    });
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, load_integer_argument);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, load_sse_argument);
    res = kefir_ir_type_visitor_list_subtrees(&func->params, &visitor, (void *) &param, 0, kefir_ir_type_length(&func->params));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(codegen->mem, &allocation);
        kefir_vector_free(codegen->mem, &layout);
        return res;
    });
    kefir_amd64_sysv_parameter_free(codegen->mem, &allocation);
    kefir_vector_free(codegen->mem, &layout);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *codegen,
                                              const struct kefir_irfunction_decl *func) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration"));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Begin prologue of %s", func->identifier));
    REQUIRE_OK(preserve_state(&codegen->asmgen));
    REQUIRE_OK(load_arguments(codegen, func));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "End prologue of %s", func->identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *codegen, const struct kefir_irfunction_decl *func) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration"));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Begin epilogue of %s", func->identifier));
    REQUIRE_OK(restore_state(&codegen->asmgen));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "End of %s", func->identifier));
    return KEFIR_OK;
}