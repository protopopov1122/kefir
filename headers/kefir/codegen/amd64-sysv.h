#ifndef KEFIR_CODEGEN_AMD64_SYSV_H_
#define KEFIR_CODEGEN_AMD64_SYSV_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ir/bytecode.h"
#include "kefir/codegen/codegen.h"
#include "kefir/codegen/amd64/asmgen.h"

#define KEFIR_CODEGEN_AMD64_BUFLEN 256
#define KEFIR_CODEGEN_AMD64_BUFNUM 3

typedef struct kefir_codegen_amd64 {
    struct kefir_codegen iface;
    struct kefir_amd64_asmgen asmgen;
    struct kefir_mem *mem;

    char buf[KEFIR_CODEGEN_AMD64_BUFNUM][KEFIR_CODEGEN_AMD64_BUFLEN + 1];
} kefir_codegen_amd64_t;

kefir_result_t kefir_codegen_amd64_sysv_init(struct kefir_codegen_amd64 *, FILE *, struct kefir_mem *);

#endif