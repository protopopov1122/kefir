#include "kefir/ir/data.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_data_alloc(struct kefir_mem *mem,
                               const struct kefir_ir_type *type,
                               struct kefir_ir_data *data) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type pointer"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    const kefir_size_t slots = kefir_ir_type_total_slots(type);
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_ir_data_value), slots, &data->value));
    REQUIRE_OK(kefir_vector_extend(&data->value, slots));
    data->type = type;
    for (kefir_size_t i = 0; i < kefir_vector_length(&data->value); i++) {
        ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, i));
        REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected to have valid entry at index"));
        entry->undefined = true;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_free(struct kefir_mem *mem,
                              struct kefir_ir_data *data) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE_OK(kefir_vector_free(mem, &data->value));
    data->type = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_i64(struct kefir_ir_data *data,
                                 kefir_size_t index,
                                 kefir_int64_t value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_data_value *entry = (struct kefir_ir_data_value *) kefir_vector_at(&data->value, index);
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->undefined = false;
    entry->i64 = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_f32(struct kefir_ir_data *data,
                                 kefir_size_t index,
                                 kefir_float32_t value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_data_value *entry = (struct kefir_ir_data_value *) kefir_vector_at(&data->value, index);
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->undefined = false;
    entry->f32[0] = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_f64(struct kefir_ir_data *data,
                                 kefir_size_t index,
                                 kefir_float64_t value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_data_value *entry = (struct kefir_ir_data_value *) kefir_vector_at(&data->value, index);
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->undefined = false;
    entry->f64 = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_data(struct kefir_ir_data *data,
                                  kefir_size_t index,
                                  const void *ptr) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_data_value *entry = (struct kefir_ir_data_value *) kefir_vector_at(&data->value, index);
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->undefined = false;
    entry->data = ptr;
    return KEFIR_OK;
}