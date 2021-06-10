#include "kefir/ir/data.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_data_alloc(struct kefir_mem *mem, const struct kefir_ir_type *type, kefir_id_t type_id,
                                   struct kefir_ir_data *data) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type pointer"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    const kefir_size_t slots = kefir_ir_type_total_slots(type);
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_ir_data_value), slots, &data->value));
    REQUIRE_OK(kefir_vector_extend(&data->value, slots));
    data->type = type;
    data->type_id = type_id;
    for (kefir_size_t i = 0; i < kefir_vector_length(&data->value); i++) {
        ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, i));
        REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected to have valid entry at index"));
        entry->type = KEFIR_IR_DATA_VALUE_UNDEFINED;
    }
    data->finalized = false;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_free(struct kefir_mem *mem, struct kefir_ir_data *data) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE_OK(kefir_vector_free(mem, &data->value));
    data->type = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_integer(struct kefir_ir_data *data, kefir_size_t index, kefir_int64_t value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE(!data->finalized, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot modify finalized data"));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, index));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->type = KEFIR_IR_DATA_VALUE_INTEGER;
    entry->value.integer = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_float32(struct kefir_ir_data *data, kefir_size_t index, kefir_float32_t value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE(!data->finalized, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot modify finalized data"));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, index));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->type = KEFIR_IR_DATA_VALUE_FLOAT32;
    entry->value.float32 = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_float64(struct kefir_ir_data *data, kefir_size_t index, kefir_float64_t value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE(!data->finalized, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot modify finalized data"));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, index));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->type = KEFIR_IR_DATA_VALUE_FLOAT64;
    entry->value.float64 = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_string(struct kefir_ir_data *data, kefir_size_t index, const char *value) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE(!data->finalized, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot modify finalized data"));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, index));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->type = KEFIR_IR_DATA_VALUE_STRING;
    entry->value.string = value;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_pointer(struct kefir_ir_data *data, kefir_size_t index, const char *reference,
                                         kefir_size_t offset) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE(!data->finalized, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot modify finalized data"));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, index));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->type = KEFIR_IR_DATA_VALUE_POINTER;
    entry->value.pointer.reference = reference;
    entry->value.pointer.offset = offset;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_set_raw(struct kefir_ir_data *data, kefir_size_t index, const void *raw,
                                     kefir_size_t length) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    REQUIRE(!data->finalized, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot modify finalized data"));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&data->value, index));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to find specified index"));
    entry->type = KEFIR_IR_DATA_VALUE_RAW;
    entry->value.raw.data = raw;
    entry->value.raw.length = length;
    return KEFIR_OK;
}

struct finalize_param {
    struct kefir_ir_type_visitor *visitor;
    struct kefir_ir_data *data;
    kefir_size_t slot;
    kefir_bool_t defined;
};

static kefir_result_t finalize_unsupported(const struct kefir_ir_type *type, kefir_size_t index,
                                           const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsupported IR data type");
}

static kefir_result_t finalize_pad(const struct kefir_ir_type *type, kefir_size_t index,
                                   const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct finalize_param *, param, payload);
    param->slot++;
    return KEFIR_OK;
}

static kefir_result_t finalize_scalar(const struct kefir_ir_type *type, kefir_size_t index,
                                      const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct finalize_param *, param, payload);

    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, param->slot++));
    param->defined = param->defined || entry->type != KEFIR_IR_DATA_VALUE_UNDEFINED;
    return KEFIR_OK;
}

static kefir_result_t finalize_struct_union(const struct kefir_ir_type *type, kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct finalize_param *, param, payload);

    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, param->slot++));
    REQUIRE(entry->type == KEFIR_IR_DATA_VALUE_UNDEFINED,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "IR data for structure/union cannot have directly assigned value"));

    struct finalize_param subparam = {
        .visitor = param->visitor, .data = param->data, .slot = param->slot, .defined = false};
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, &subparam, index + 1, typeentry->param));
    param->slot = subparam.slot;
    param->defined = param->defined || subparam.defined;
    if (subparam.defined) {
        entry->type = KEFIR_IR_DATA_VALUE_AGGREGATE;
    }

    return KEFIR_OK;
}

static kefir_result_t finalize_array(const struct kefir_ir_type *type, kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct finalize_param *, param, payload);

    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, param->slot++));
    struct finalize_param subparam = {
        .visitor = param->visitor, .data = param->data, .slot = param->slot, .defined = false};
    for (kefir_size_t i = 0; i < (kefir_size_t) typeentry->param; i++) {
        REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, &subparam, index + 1, 1));
    }
    param->slot = subparam.slot;
    param->defined = param->defined || subparam.defined;
    if (subparam.defined) {
        REQUIRE(entry->type == KEFIR_IR_DATA_VALUE_UNDEFINED,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Array data cannot simultaneously have directly assigned and aggregate values"));
        entry->type = KEFIR_IR_DATA_VALUE_AGGREGATE;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ir_data_finalize(struct kefir_ir_data *data) {
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_type_visitor visitor;
    struct finalize_param param = {.visitor = &visitor, .data = data, .slot = 0, .defined = false};

    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, finalize_unsupported));
    KEFIR_IR_TYPE_VISITOR_INIT_SCALARS(&visitor, finalize_scalar);
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = finalize_scalar;
    visitor.visit[KEFIR_IR_TYPE_PAD] = finalize_pad;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = finalize_struct_union;
    visitor.visit[KEFIR_IR_TYPE_UNION] = finalize_struct_union;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = finalize_array;
    REQUIRE_OK(
        kefir_ir_type_visitor_list_nodes(data->type, &visitor, &param, 0, kefir_ir_type_total_length(data->type)));
    data->finalized = true;
    return KEFIR_OK;
}
