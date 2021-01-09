#ifndef KEFIR_CODEGEN_AMD64_SYSV_H_
#define KEFIR_CODEGEN_AMD64_SYSV_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ir/bytecode.h"
#include "kefir/codegen/codegen.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_codegen_amd64 {
    struct kefir_codegen iface;
    struct kefir_amd64_asmgen asmgen;
    struct kefir_mem *mem;
} kefir_codegen_amd64_t;

typedef struct kefir_codegen_amd64_sysv_module {
    const struct kefir_ir_module *module;
    struct kefir_hashtree function_gates;
} kefir_codegen_amd64_sysv_module_t;

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *, FILE *, struct kefir_mem *);

#endif