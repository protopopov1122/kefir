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
    struct {
        struct kefir_vector layout;
        struct kefir_vector allocation;
        struct kefir_amd64_sysv_parameter_location location;
    } parameters;

    struct {
        kefir_size_t size;
        kefir_size_t alignment;
    } frame;
} kefir_amd64_sysv_function_t;

kefir_result_t kefir_amd64_sysv_function_alloc(struct kefir_mem *, const struct kefir_irfunction *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_free(struct kefir_mem *, struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_prologue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);
kefir_result_t kefir_amd64_sysv_function_epilogue(struct kefir_codegen_amd64 *, const struct kefir_amd64_sysv_function *);

#endif