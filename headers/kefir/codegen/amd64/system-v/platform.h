#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_PLATFORM_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_PLATFORM_H_

#include "kefir/ir/platform.h"
#include "kefir/codegen/amd64/system-v/abi/module.h"

typedef struct kefir_codegen_amd64_sysv_type {
    const struct kefir_ir_type *ir_type;
    struct kefir_vector layout;
} kefir_codegen_amd64_sysv_type_t;

kefir_result_t kefir_codegen_amd64_sysv_target_platform(struct kefir_ir_target_platform *);

#endif
