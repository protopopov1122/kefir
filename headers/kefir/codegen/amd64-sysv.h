#ifndef KEFIR_CODEGEN_AMD64_SYSV_H_
#define KEFIR_CODEGEN_AMD64_SYSV_H_

#include "kefir/core/basic-types.h"
#include "kefir/codegen/amd64/system-v/abi/module.h"
#include "kefir/codegen/amd64/system-v/platform.h"

typedef struct kefir_codegen_amd64 {
    struct kefir_codegen iface;
    struct kefir_amd64_asmgen asmgen;
} kefir_codegen_amd64_t;

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *, FILE *);

#endif
