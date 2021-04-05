#ifndef KEFIR_UTIL_JSON_H_
#define KEFIR_UTIL_JSON_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"

#define KEFIR_JSON_MAX_DEPTH 1024

typedef enum kefir_json_state {
    KEFIR_JSON_STATE_INIT = 0,
    KEFIR_JSON_STATE_OBJECT_EMPTY,
    KEFIR_JSON_STATE_OBJECT_FIELD,
    KEFIR_JSON_STATE_OBJECT,
    KEFIR_JSON_STATE_ARRAY_EMPTY,
    KEFIR_JSON_STATE_ARRAY,
} kefir_json_state_t;

typedef struct kefir_json_output {
    FILE *file;
    kefir_size_t indent;
    kefir_json_state_t state[KEFIR_JSON_MAX_DEPTH];
    kefir_size_t level;
} kefir_json_output_t;

kefir_result_t kefir_json_output_init(struct kefir_json_output *, FILE *, kefir_size_t);
kefir_result_t kefir_json_output_finalize(struct kefir_json_output *);

kefir_result_t kefir_json_output_object_begin(struct kefir_json_output *);
kefir_result_t kefir_json_output_object_end(struct kefir_json_output *);
kefir_result_t kefir_json_output_array_begin(struct kefir_json_output *);
kefir_result_t kefir_json_output_array_end(struct kefir_json_output *);

kefir_result_t kefir_json_output_object_key(struct kefir_json_output *, const char *);
kefir_result_t kefir_json_output_integer(struct kefir_json_output *, kefir_int64_t);
kefir_result_t kefir_json_output_uinteger(struct kefir_json_output *, kefir_uint64_t);
kefir_result_t kefir_json_output_float(struct kefir_json_output *, kefir_float64_t);
kefir_result_t kefir_json_output_string(struct kefir_json_output *, const char *);
kefir_result_t kefir_json_output_boolean(struct kefir_json_output *, kefir_bool_t);
kefir_result_t kefir_json_output_null(struct kefir_json_output *);


#endif