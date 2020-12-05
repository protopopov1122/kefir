#include <string.h>
#include "kefir/ir/type.h"
#include "kefir/core/util.h"

kefir_result_t kefir_ir_type_init(struct kefir_ir_type *type, void *area, kefir_size_t capacity) {
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE((area != NULL && capacity != 0) || (area == NULL && capacity == 0), KEFIR_MALFORMED_ARG);
    return kefir_vector_init(&type->vector, sizeof(struct kefir_ir_typeentry), area, capacity);
}

kefir_size_t kefir_ir_type_available(const struct kefir_ir_type *type) {
    REQUIRE(type != NULL, 0);
    return kefir_vector_available(&type->vector);
}

kefir_size_t kefir_ir_type_length(const struct kefir_ir_type *type) {
    REQUIRE(type != NULL, 0);
    return kefir_vector_length(&type->vector);
}

struct kefir_ir_typeentry *kefir_ir_type_at(const struct kefir_ir_type *type, kefir_size_t index) {
    REQUIRE(type != NULL, NULL);
    return (struct kefir_ir_typeentry *) kefir_vector_at(&type->vector, index);
}

kefir_result_t kefir_ir_type_append(struct kefir_ir_type *type, const struct kefir_ir_typeentry *value) {
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(value != NULL, KEFIR_MALFORMED_ARG);
    return kefir_vector_append(&type->vector, value);
}

kefir_result_t kefir_ir_type_append_v(struct kefir_ir_type *type, kefir_ir_typecode_t typecode, kefir_uint32_t alignment, kefir_int32_t param) {
    struct kefir_ir_typeentry typeentry = {
        .typecode = typecode,
        .alignment = alignment,
        .param = param
    };
    return kefir_ir_type_append(type, &typeentry);
}

kefir_result_t kefir_ir_type_alloc(struct kefir_mem *mem, kefir_size_t capacity, struct kefir_ir_type *type) {
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    if (capacity == 0) {
        return kefir_ir_type_init(type, NULL, 0);
    }
    return kefir_vector_alloc(mem, sizeof(struct kefir_ir_typeentry), capacity, &type->vector);
}

kefir_result_t kefir_ir_type_realloc(struct kefir_mem *mem, kefir_size_t capacity, struct kefir_ir_type *type) {
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    if (capacity == 0) {
        return kefir_ir_type_free(mem, type);
    }
    return kefir_vector_realloc(mem, capacity, &type->vector);
}

kefir_result_t kefir_ir_type_free(struct kefir_mem *mem, struct kefir_ir_type *type) {
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    return kefir_vector_free(mem, &type->vector);
}

kefir_result_t kefir_ir_type_visitor_init(struct kefir_ir_type_visitor *visitor, kefir_ir_type_visitor_callback_t defCallback) {
    REQUIRE(visitor != NULL, KEFIR_MALFORMED_ARG);
    for (kefir_size_t i = 0; i < KEFIR_IR_TYPE_COUNT; i++) {
        visitor->visit[i] = defCallback;
    }
    return KEFIR_OK;
}

static kefir_size_t length_of(const struct kefir_ir_type *type, kefir_size_t index) {
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, index);   
    REQUIRE(typeentry != NULL, 0);
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_STRUCT:
        case KEFIR_IR_TYPE_UNION: {
            kefir_size_t length = 1;
            kefir_size_t counter = typeentry->param;
            while (counter--) {
                length += length_of(type, index + length);
            }
            return length;
        };

        case KEFIR_IR_TYPE_ARRAY:
            return length_of(type, index + 1) + 1;
        
        default:
            return 1;
    }
}

kefir_result_t kefir_ir_type_visitor_traverse(const struct kefir_ir_type *type,
                                          const struct kefir_ir_type_visitor *visitor,
                                          void *payload,
                                          kefir_size_t begin,
                                          kefir_size_t length) {

    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(visitor != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(begin < kefir_ir_type_length(type), KEFIR_OUT_OF_BOUNDS);
    REQUIRE(length > 0, KEFIR_MALFORMED_ARG);
    for (kefir_size_t index = begin; index < kefir_ir_type_length(type) && length-- > 0; index += length_of(type, index)) {
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, index);
        REQUIRE(typeentry !=NULL, KEFIR_MALFORMED_ARG);
#define INVOKE(method) \
    do { \
        if (method) { \
            REQUIRE_OK((method)((type), (index), (typeentry->typecode), (typeentry->param), (payload))); \
        } \
    } while (0)
        switch (typeentry->typecode) {
            case KEFIR_IR_TYPE_STRUCT:
                REQUIRE(typeentry->param > 0, KEFIR_MALFORMED_ARG);
                INVOKE(visitor->visit[KEFIR_IR_TYPE_STRUCT]);
                break;

            case KEFIR_IR_TYPE_ARRAY:
                INVOKE(visitor->visit[KEFIR_IR_TYPE_ARRAY]);
                break;

            case KEFIR_IR_TYPE_UNION:
                REQUIRE(typeentry->param > 0, KEFIR_MALFORMED_ARG);
                INVOKE(visitor->visit[KEFIR_IR_TYPE_UNION]);
                break;

            default:
                INVOKE(visitor->visit[typeentry->typecode]);
                break;
        }
#undef INVOKE
    }
    return KEFIR_OK;
}