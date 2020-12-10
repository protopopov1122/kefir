#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_ALLOCATION_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_ALLOCATION_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64/system-v/abi_data.h"

typedef struct kefir_amd64_sysv_abi_qword {
    kefir_amd64_sysv_data_class_t klass;
    kefir_size_t index;
    kefir_size_t current_offset;
} kefir_amd64_sysv_abi_qword_t;

typedef struct kefir_amd64_sysv_abi_qword_ref {
    struct kefir_amd64_sysv_abi_qword *qword;
    kefir_size_t offset;
} kefir_amd64_sysv_abi_qword_ref_t;

typedef struct kefir_amd64_sysv_abi_qwords {
    struct kefir_vector qwords;
    kefir_size_t current;
} kefir_amd64_sysv_abi_qwords_t;

kefir_result_t kefir_amd64_sysv_abi_qwords_count(const struct kefir_ir_type *,
                                             const struct kefir_vector *,
                                             kefir_size_t *);

kefir_result_t kefir_amd64_sysv_abi_qwords_alloc(struct kefir_amd64_sysv_abi_qwords *,
                                             struct kefir_mem *, 
                                             kefir_size_t);

kefir_result_t kefir_amd64_sysv_abi_qwords_free(struct kefir_amd64_sysv_abi_qwords *,
                                            struct kefir_mem *);

kefir_result_t kefir_amd64_sysv_abi_qwords_next(struct kefir_amd64_sysv_abi_qwords *,
                                            kefir_amd64_sysv_data_class_t,
                                            kefir_size_t,
                                            kefir_size_t,
                                            struct kefir_amd64_sysv_abi_qword_ref *);

kefir_result_t kefir_amd64_sysv_abi_qwords_reset_class(struct kefir_amd64_sysv_abi_qwords *,
                                                    kefir_amd64_sysv_data_class_t,
                                                    kefir_size_t,
                                                    kefir_size_t);

typedef enum kefir_amd64_sysv_input_parameter_type {
    KEFIR_AMD64_SYSV_INPUT_PARAM_DIRECT,
    KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED_DIRECT,
    KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER,
    KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER,
    KEFIR_AMD64_SYSV_INPUT_PARAM_SKIP
} kefir_amd64_sysv_input_parameter_type_t;

typedef struct kefir_amd64_sysv_input_parameter_allocation {
    kefir_amd64_sysv_input_parameter_type_t type;
    kefir_amd64_sysv_data_class_t klass;
    union {
        struct kefir_amd64_sysv_abi_qwords container;
        struct kefir_amd64_sysv_abi_qword_ref container_reference;
    };
} kefir_amd64_sysv_input_parameter_allocation_t;

kefir_result_t kefir_amd64_sysv_input_parameter_allocate(struct kefir_mem *,
                                                     const struct kefir_ir_type *,
                                                     const struct kefir_vector *,
                                                     struct kefir_vector *);

kefir_result_t kefir_amd64_sysv_input_parameter_free(struct kefir_mem *,
                                                 struct kefir_vector *);

#endif