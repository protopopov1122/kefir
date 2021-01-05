#ifndef KEFIR_IR_DATA_H_
#define KEFIR_IR_DATA_H_

#include <stdbool.h>
#include "kefir/ir/type.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"

typedef struct kefir_ir_data {
    const struct kefir_ir_type *type;
    struct kefir_vector value;
    kefir_size_t size;
    kefir_size_t alignment;
} kefir_ir_data_t;

typedef struct kefir_ir_data_value {
    bool undefined;
    union {
        kefir_int64_t i64;
        kefir_float32_t f32[2];
        kefir_float64_t f64;
        const char *str;
        void *data;
    };
} kefir_ir_data_value_t;

kefir_result_t kefir_ir_data_alloc(struct kefir_mem *,
                               const struct kefir_ir_type *,
                               struct kefir_ir_data *);
kefir_result_t kefir_ir_data_free(struct kefir_mem *,
                              struct kefir_ir_data *);
kefir_result_t kefir_ir_data_set_i64(struct kefir_ir_data *,
                                 kefir_size_t,
                                 kefir_int64_t);
kefir_result_t kefir_ir_data_set_f32(struct kefir_ir_data *,
                                 kefir_size_t,
                                 kefir_float32_t);
kefir_result_t kefir_ir_data_set_f64(struct kefir_ir_data *,
                                 kefir_size_t,
                                 kefir_float64_t);
kefir_result_t kefir_ir_data_set_string(struct kefir_ir_data *,
                                    kefir_size_t,
                                    const char *);
kefir_result_t kefir_ir_data_set_data(struct kefir_ir_data *,
                                  kefir_size_t,
                                  void *);

#endif