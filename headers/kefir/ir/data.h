#ifndef KEFIR_IR_DATA_H_
#define KEFIR_IR_DATA_H_

#include <stdbool.h>
#include "kefir/ir/type.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"

typedef struct kefir_ir_data {
    kefir_id_t type_id;
    const struct kefir_ir_type *type;
    struct kefir_vector value;
    kefir_bool_t finalized;
} kefir_ir_data_t;

typedef enum kefir_ir_data_value_type {
    KEFIR_IR_DATA_VALUE_UNDEFINED,
    KEFIR_IR_DATA_VALUE_INTEGER,
    KEFIR_IR_DATA_VALUE_FLOAT32,
    KEFIR_IR_DATA_VALUE_FLOAT64,
    KEFIR_IR_DATA_VALUE_STRING,
    KEFIR_IR_DATA_VALUE_POINTER,
    KEFIR_IR_DATA_VALUE_RAW,
    KEFIR_IR_DATA_VALUE_AGGREGATE
} kefir_ir_data_value_type_t;

typedef struct kefir_ir_data_value {
    kefir_ir_data_value_type_t type;
    union {
        kefir_int64_t integer;
        kefir_float32_t float32;
        kefir_float64_t float64;
        const char *string;
        struct {
            const char *reference;
            kefir_int64_t offset;
        } pointer;
        struct {
            const void *data;
            kefir_size_t length;
        } raw;
    } value;
} kefir_ir_data_value_t;

kefir_result_t kefir_ir_data_alloc(struct kefir_mem *,
                               const struct kefir_ir_type *,
                               kefir_id_t,
                               struct kefir_ir_data *);

kefir_result_t kefir_ir_data_free(struct kefir_mem *,
                              struct kefir_ir_data *);

kefir_result_t kefir_ir_data_set_integer(struct kefir_ir_data *,
                                     kefir_size_t,
                                     kefir_int64_t);

kefir_result_t kefir_ir_data_set_float32(struct kefir_ir_data *,
                                     kefir_size_t,
                                     kefir_float32_t);

kefir_result_t kefir_ir_data_set_float64(struct kefir_ir_data *,
                                     kefir_size_t,
                                     kefir_float64_t);

kefir_result_t kefir_ir_data_set_string(struct kefir_ir_data *,
                                     kefir_size_t,
                                     const char *);

kefir_result_t kefir_ir_data_set_pointer(struct kefir_ir_data *,
                                     kefir_size_t,
                                     const char *,
                                     kefir_size_t);

kefir_result_t kefir_ir_data_set_raw(struct kefir_ir_data *,
                                 kefir_size_t,
                                 const void *,
                                 kefir_size_t);

kefir_result_t kefir_ir_data_finalize(struct kefir_ir_data *);

#endif