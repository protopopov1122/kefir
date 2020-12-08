#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_ALLOCATION_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_ALLOCATION_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"

typedef struct kefir_amd64_sysv_abi_qword {
    kefir_amd64_sysv_data_class_t klass;
    kefir_size_t offset;
} kefir_amd64_sysv_abi_qword_t;

typedef struct kefir_amd64_sysv_abi_qword_ref {
    kefir_size_t index;
    kefir_size_t offset;
} kefir_amd64_sysv_abi_qword_ref_t;

typedef struct kefir_amd64_sysv_abi_qwords {
    struct kefir_vector qwords;
    kefir_size_t current;

    const struct kefir_ir_type *type;
    const struct kefir_vector *layout;
} kefir_amd64_sysv_abi_qwords_t;

kefir_result_t kefir_amd64_sysv_abi_qwords_alloc(struct kefir_amd64_sysv_abi_qwords *,
                                             struct kefir_mem *, 
                                             const struct kefir_ir_type *,
                                             const struct kefir_vector *);
kefir_result_t kefir_amd64_sysv_abi_qwords_free(struct kefir_amd64_sysv_abi_qwords *,
                                            struct kefir_mem *);
kefir_result_t kefir_amd64_sysv_abi_qwords_next(struct kefir_amd64_sysv_abi_qwords *,
                                            kefir_amd64_sysv_data_class_t,
                                            kefir_size_t,
                                            kefir_size_t,
                                            struct kefir_amd64_sysv_abi_qword_ref *);


#endif