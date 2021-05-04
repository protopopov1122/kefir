#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_LAYOUT_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_LAYOUT_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/ir/type.h"

typedef struct kefir_amd64_sysv_data_layout {
    kefir_size_t size;
    kefir_size_t alignment;
    bool aligned;
    kefir_size_t relative_offset;
} kefir_amd64_sysv_data_layout_t;

kefir_result_t kefir_amd64_sysv_scalar_type_layout(kefir_ir_typecode_t,
                                               kefir_size_t *,
                                               kefir_size_t *);

kefir_result_t kefir_amd64_sysv_type_layout(const struct kefir_ir_type *,
                                        struct kefir_mem *,
                                        struct kefir_vector *);

#endif