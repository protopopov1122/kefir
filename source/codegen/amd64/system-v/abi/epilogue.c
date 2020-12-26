#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/util.h"
#include <stdio.h>

struct result_return {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_amd64_sysv_function *func;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Unsupported function return type");
}

static kefir_result_t return_integer(const struct kefir_ir_type *type,
                                   kefir_size_t index,
                                   const struct kefir_ir_typeentry *typeentry,
                                   void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct result_return *param = (struct result_return *) payload;
    ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_POP, KEFIR_AMD64_RAX);
    return KEFIR_OK;  
}

static kefir_result_t return_float(const struct kefir_ir_type *type,
                                   kefir_size_t index,
                                   const struct kefir_ir_typeentry *typeentry,
                                   void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct result_return *param = (struct result_return *) payload;
    ASMGEN_INSTR1(&param->codegen->asmgen, KEFIR_AMD64_POP, KEFIR_AMD64_SYSV_ABI_DATA_REG);
    ASMGEN_INSTR3(&param->codegen->asmgen, KEFIR_AMD64_PINSRQ,
        KEFIR_AMD64_XMM0,
        KEFIR_AMD64_SYSV_ABI_DATA_REG,
        "0");
    return KEFIR_OK;  
}

static kefir_result_t return_values(struct kefir_codegen_amd64 *codegen,
                                  const struct kefir_amd64_sysv_function *func) {
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, return_integer);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, return_float);
    struct result_return param = {
        .codegen = codegen,
        .func = func
    };
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(&func->func->declaration.result, &visitor, (void *) &param, 0, 1));
    return KEFIR_OK;
}

static kefir_result_t restore_state(struct kefir_codegen_amd64 *codegen,
                                  const struct kefir_amd64_sysv_function *func) {
    ASMGEN_INSTR2(&codegen->asmgen, KEFIR_AMD64_MOV, KEFIR_AMD64_RSP, KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG);
    if (func->frame.size > 0) {
        ASMGEN_INSTR2(&codegen->asmgen, KEFIR_AMD64_SUB, KEFIR_AMD64_RSP,
            FORMAT(codegen->buf[0], "%lu", func->frame.size));
    }
    ASMGEN_INSTR1(&codegen->asmgen, KEFIR_AMD64_JMP, KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *codegen,
                                              const struct kefir_amd64_sysv_function *func) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration"));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Begin epilogue of %s", func->func->declaration.identifier));
    REQUIRE_OK(return_values(codegen, func));
    REQUIRE_OK(restore_state(codegen, func));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "End of %s", func->func->declaration.identifier));
    return KEFIR_OK;
}