#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/amd64/asmgen.h"
#include "kefir/codegen/amd64/system-v/abi/function.h"
#include "kefir/codegen/amd64/system-v/abi/static_data.h"

typedef enum kefir_amd64_sysv_internal_fields {
    KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS = 0,
    // Auxilary
    KEFIR_AMD64_SYSV_INTERNAL_COUNT = 2
} kefir_amd64_sysv_internal_fields_t;

_Static_assert(KEFIR_AMD64_SYSV_INTERNAL_COUNT % 2 == 0, "KEFIR_AMD64_SYSV_INTERNAL_COUNT must be divisible by 2");

#define KEFIR_AMD64_SYSV_INTERNAL_BOUND (KEFIR_AMD64_SYSV_INTERNAL_COUNT * KEFIR_AMD64_SYSV_ABI_QWORD)

#define KEFIR_AMD64_SYSV_ABI_PROGRAM_REG KEFIR_AMD64_RBX
#define KEFIR_AMD64_SYSV_ABI_TMP_REG KEFIR_AMD64_R11
#define KEFIR_AMD64_SYSV_ABI_DATA_REG KEFIR_AMD64_R12
#define KEFIR_AMD64_SYSV_ABI_DATA2_REG KEFIR_AMD64_R13
#define KEFIR_AMD64_SYSV_ABI_STACK_BASE_REG KEFIR_AMD64_R14

#endif
