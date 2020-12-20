#ifndef KEFIR_CODEGEN_AMD6_SYSTEM_V_ABI_QWORDS_H_
#define KEFIR_CODEGEN_AMD6_SYSTEM_V_ABI_QWORDS_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/data.h"

#define KEFIR_AMD64_SYSV_ABI_QWORD 8

typedef struct kefir_amd64_sysv_abi_qword {
    kefir_size_t index;
    kefir_amd64_sysv_data_class_t klass;
    kefir_size_t location;
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

typedef struct kefir_amd64_sysv_abi_qword_position {
    kefir_size_t index;
    kefir_size_t offset;
} kefir_amd64_sysv_abi_qword_position_t;

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

kefir_result_t kefir_amd64_sysv_abi_qwords_save_position(const struct kefir_amd64_sysv_abi_qwords *,
                                                     struct kefir_amd64_sysv_abi_qword_position *);

kefir_result_t kefir_amd64_sysv_abi_qwords_restore_position(struct kefir_amd64_sysv_abi_qwords *,
                                                        const struct kefir_amd64_sysv_abi_qword_position *);

kefir_result_t kefir_amd64_sysv_abi_qwords_max_position(const struct kefir_amd64_sysv_abi_qword_position *,
                                                    const struct kefir_amd64_sysv_abi_qword_position *,
                                                    struct kefir_amd64_sysv_abi_qword_position *);

#endif