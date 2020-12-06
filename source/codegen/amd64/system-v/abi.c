#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"
#include <stdio.h>

static kefir_result_t preserve_state(struct kefir_amd64_asmgen *asmgen) {
    ASMGEN_INSTR1(asmgen, KEFIR_AMD64_CALL, KEFIR_AMD64_SYSTEM_V_RUNTIME_PRESERVE_STATE);
    return KEFIR_OK;
}

static kefir_result_t restore_state(struct kefir_amd64_asmgen *asmgen) {
    ASMGEN_INSTR1(asmgen, KEFIR_AMD64_JMP, KEFIR_AMD64_SYSTEM_V_RUNTIME_RESTORE_STATE);
    return KEFIR_OK;
}

static kefir_result_t load_argument(struct kefir_codegen_amd64 *codegen,
                                  const struct kefir_irfunction_decl *func,
                                  kefir_size_t index,
                                  struct kefir_amd64_sysv_parameter_allocation *param) {
    UNUSED(func);
    UNUSED(index);
    switch (param->dataclass) {
        case KEFIR_AMD64_SYSV_PARAM_INTEGER:
            ASMGEN_INSTR1(&codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_SYSV_INTEGER_REGISTERS[param->registers[0]]);
            break;

        case KEFIR_AMD64_SYSV_PARAM_SSE:
            ASMGEN_INSTR3(&codegen->asmgen, KEFIR_AMD64_PEXTRQ, KEFIR_AMD64_R12, KEFIR_AMD64_SYSV_SSE_REGISTERS[param->registers[0]], "0");
            ASMGEN_INSTR1(&codegen->asmgen, KEFIR_AMD64_PUSH, KEFIR_AMD64_R12);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Non-scalar function parameter is not supported");
    }
    return KEFIR_OK;
}

static kefir_result_t load_arguments(struct kefir_codegen_amd64 *codegen,
                                   const struct kefir_irfunction_decl *func) {
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Load parameters of %s", func->identifier));
    struct kefir_vector layout, allocation;
    REQUIRE_OK(kefir_amd64_sysv_type_layout(&func->params, codegen->mem, &layout));
    kefir_result_t res = kefir_amd64_sysv_type_param_allocation(&func->params, codegen->mem, &layout, &allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(codegen->mem, &layout);
        return res;
    });
    for (kefir_size_t i = 0; i < kefir_ir_type_length(&func->params); i++) {
        struct kefir_amd64_sysv_parameter_allocation *param =
            (struct kefir_amd64_sysv_parameter_allocation *) kefir_vector_at(&allocation, i);
        kefir_result_t res = load_argument(codegen, func, i, param);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_vector_free(codegen->mem, &allocation);
            kefir_vector_free(codegen->mem, &layout);
            return res;
        });
    }
    kefir_vector_free(codegen->mem, &allocation);
    kefir_vector_free(codegen->mem, &layout);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *codegen,
                                              const struct kefir_irfunction_decl *func) {
    REQUIRE(codegen != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(func != NULL, KEFIR_MALFORMED_ARG);
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Begin prologue of %s", func->identifier));
    REQUIRE_OK(preserve_state(&codegen->asmgen));
    REQUIRE_OK(load_arguments(codegen, func));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "End prologue of %s", func->identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *codegen, const struct kefir_irfunction_decl *func) {
    REQUIRE(codegen != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(func != NULL, KEFIR_MALFORMED_ARG);
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "Begin epilogue of %s", func->identifier));
    REQUIRE_OK(restore_state(&codegen->asmgen));
    ASMGEN_COMMENT(&codegen->asmgen, FORMAT(codegen->buf[0], "End of %s", func->identifier));
    return KEFIR_OK;
}