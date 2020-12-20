#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/ir/function.h"

typedef struct kefir_amd64_sysv_function {
    const struct kefir_irfunction *func;
    struct kefir_vector parameter_layout;
    struct kefir_vector parameter_allocation;
    struct kefir_amd64_sysv_parameter_location parameter_requirements;
} kefir_amd64_sysv_function_t;

kefir_result_t kefir_amd64_sysv_function_alloc(struct kefir_mem *, const struct kefir_irfunction *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_free(struct kefir_mem *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);

#endif