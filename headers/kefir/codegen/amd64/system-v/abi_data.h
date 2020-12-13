#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/core/vector.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/ir/type.h"

extern const char *KEFIR_AMD64_SYSV_INTEGER_REGISTERS[];
extern kefir_size_t KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT;
extern const char *KEFIR_AMD64_SYSV_SSE_REGISTERS[];
extern kefir_size_t KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT;

typedef enum kefir_amd64_sysv_data_class {
    KEFIR_AMD64_SYSV_PARAM_INTEGER = 0,
    KEFIR_AMD64_SYSV_PARAM_SSE,
    KEFIR_AMD64_SYSV_PARAM_SSEUP,
    KEFIR_AMD64_SYSV_PARAM_X87,
    KEFIR_AMD64_SYSV_PARAM_X87UP,
    KEFIR_AMD64_SYSV_PARAM_COMPLEX_X87,
    KEFIR_AMD64_SYSV_PARAM_NO_CLASS,
    KEFIR_AMD64_SYSV_PARAM_MEMORY
} kefir_amd64_sysv_data_class_t;

typedef struct kefir_amd64_sysv_data_layout {
    kefir_size_t size;
    kefir_size_t alignment;
    bool aligned;
    kefir_size_t relative_offset;
} kefir_amd64_sysv_data_layout_t;

typedef struct kefir_amd64_sysv_parameter_allocation {
    kefir_amd64_sysv_data_class_t dataclass;
    kefir_size_t eightbyte;
    kefir_size_t eightbyte_offset;

    kefir_uint32_t location;
    kefir_uint32_t uplocation;
    kefir_uint64_t offset;
} kefir_amd64_sysv_parameter_allocation_t;

kefir_result_t kefir_amd64_sysv_type_layout(const struct kefir_ir_type *,
                                          struct kefir_mem *,
                                          struct kefir_vector *);

#define KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "AMD64 System-V ABI: "

#endif