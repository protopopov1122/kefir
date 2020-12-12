#include <string.h>
#include <stdbool.h>
#include "kefir/ir/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

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
    REQUIRE(value->typecode != KEFIR_IR_TYPE_COUNT,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot append auxilary typecodes"));
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

kefir_size_t kefir_ir_type_subtree_length(const struct kefir_ir_type *type, kefir_size_t index) {
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, index);   
    REQUIRE(typeentry != NULL, 0);
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_STRUCT:
        case KEFIR_IR_TYPE_UNION: {
            kefir_size_t length = 1;
            kefir_size_t counter = typeentry->param;
            while (counter--) {
                length += kefir_ir_type_subtree_length(type, index + length);
            }
            return length;
        };

        case KEFIR_IR_TYPE_ARRAY:
            return kefir_ir_type_subtree_length(type, index + 1) + 1;
        
        default:
            return 1;
    }
}

struct slot_count {
    struct kefir_ir_type_visitor *visitor;
    kefir_size_t count;
};

static kefir_result_t scalar_slots(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct slot_count *count = (struct slot_count *) payload;
    count->count++;
    return KEFIR_OK;
}

static kefir_result_t struct_union_slots(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    struct slot_count *count = (struct slot_count *) payload;
    count->count++;
    return kefir_ir_type_visitor_traverse_subtrees(
            type, count->visitor, payload, index + 1, typeentry->param);
}

static kefir_result_t array_slots(const struct kefir_ir_type *type,
                               kefir_size_t index,
                               const struct kefir_ir_typeentry *typeentry,
                               void *payload) {
    struct slot_count *count = (struct slot_count *) payload;
    count->count++;
    struct slot_count nested = {
        .visitor = count->visitor,
        .count = 0
    };
    REQUIRE_OK(kefir_ir_type_visitor_traverse_subtrees(
            type, count->visitor, (void *) &nested, index + 1, 1));
    count->count += nested.count * typeentry->param;
    return KEFIR_OK;
}

kefir_size_t kefir_ir_type_subtree_slots(const struct kefir_ir_type *type, kefir_size_t index) {
    REQUIRE(type != NULL, 0);
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, scalar_slots);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = struct_union_slots;
    visitor.visit[KEFIR_IR_TYPE_UNION] = struct_union_slots;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = array_slots;
    struct slot_count count = {
        .visitor = &visitor,
        .count = 0
    };
    REQUIRE(kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &count, index, 1) == KEFIR_OK, 0);
    return count.count;
}

kefir_size_t kefir_ir_type_total_slots(const struct kefir_ir_type *type) {
    REQUIRE(type != NULL, 0);
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, scalar_slots);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = struct_union_slots;
    visitor.visit[KEFIR_IR_TYPE_UNION] = struct_union_slots;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = array_slots;
    struct slot_count count = {
        .visitor = &visitor,
        .count = 0
    };
    REQUIRE(kefir_ir_type_visitor_traverse_subtrees(
        type, &visitor, (void *) &count, 0, kefir_ir_type_length(type)) == KEFIR_OK, 0);
    return count.count;
}

kefir_result_t kefir_ir_type_visitor_traverse_subtrees(const struct kefir_ir_type *type,
                                          const struct kefir_ir_type_visitor *visitor,
                                          void *payload,
                                          kefir_size_t begin,
                                          kefir_size_t count) {

    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(visitor != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(begin < kefir_ir_type_length(type), KEFIR_OUT_OF_BOUNDS);
    REQUIRE(count > 0, KEFIR_MALFORMED_ARG);
    for (kefir_size_t index = begin;
        index < kefir_ir_type_length(type) && count-- > 0;
        index += kefir_ir_type_subtree_length(type, index)) {

        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, index);
        REQUIRE(typeentry !=NULL, KEFIR_MALFORMED_ARG);
#define INVOKE(method) \
    do { \
        if (method) { \
            REQUIRE_OK((method)((type), (index), (typeentry), (payload))); \
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

struct iter_params {
    struct kefir_ir_type_iterator *iter;
    kefir_size_t counter;
    kefir_size_t slot;
    kefir_size_t target;
};

static kefir_result_t iter_visitor(const struct kefir_ir_type *type,
                                 kefir_size_t index,
                                 const struct kefir_ir_typeentry *typeentry,
                                 void *payload) {
    UNUSED(typeentry);
    struct iter_params *params =
        (struct iter_params *) payload;
    if (params->counter < params->target) {
        params->slot += kefir_ir_type_subtree_slots(type, index);
        params->counter++;
        return KEFIR_OK;
    } else if (index == params->target) {
        params->iter->index = index;
        params->iter->slot = params->slot;
        return KEFIR_YIELD;
    } else {
        return KEFIR_NOT_FOUND;
    }
}

kefir_result_t kefir_ir_type_iterator_init(const struct kefir_ir_type *type, struct kefir_ir_type_iterator *iter) {
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(iter != NULL, KEFIR_MALFORMED_ARG);
    iter->type = type;
    iter->index = 0;
    iter->slot = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_type_iterator_goto(struct kefir_ir_type_iterator *iter, kefir_size_t target) {
    REQUIRE(iter != NULL, KEFIR_MALFORMED_ARG);
    iter->index = 0;
    iter->slot = 0;
    struct iter_params params = {
        .iter = iter,
        .slot = 0,
        .counter = 0,
        .target = target
    };
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, iter_visitor);
    REQUIRE_YIELD(kefir_ir_type_visitor_traverse_subtrees(
        iter->type, &visitor, (void *) &params, 0, kefir_ir_type_length(iter->type)), KEFIR_NOT_FOUND);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_type_iterator_goto_field(struct kefir_ir_type_iterator *iter, kefir_size_t target) {
    REQUIRE(iter != NULL, KEFIR_MALFORMED_ARG);
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(iter->type, iter->index);
    REQUIRE(typeentry != NULL, KEFIR_OUT_OF_BOUNDS);
    REQUIRE(typeentry->typecode == KEFIR_IR_TYPE_STRUCT ||
        typeentry->typecode == KEFIR_IR_TYPE_UNION,
        KEFIR_MALFORMED_ARG);
    struct iter_params params = {
        .iter = iter,
        .slot = 1 + iter->slot,
        .counter = 0,
        .target = target
    };
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, iter_visitor);
    REQUIRE_YIELD(kefir_ir_type_visitor_traverse_subtrees(
        iter->type, &visitor, (void *) &params, iter->index + 1, kefir_ir_type_subtree_length(iter->type, iter->index)),
        KEFIR_NOT_FOUND);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_type_iterator_goto_index(struct kefir_ir_type_iterator *iter, kefir_size_t target) {
    REQUIRE(iter != NULL, KEFIR_MALFORMED_ARG);
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(iter->type, iter->index);
    REQUIRE(typeentry != NULL, KEFIR_OUT_OF_BOUNDS);
    REQUIRE(typeentry->typecode == KEFIR_IR_TYPE_ARRAY, KEFIR_MALFORMED_ARG);
    REQUIRE(target < (kefir_size_t) typeentry->param, KEFIR_OUT_OF_BOUNDS);
    iter->index += iter->index;
    iter->slot += 1 + kefir_ir_type_subtree_slots(iter->type, iter->index + 1) * target;
    return KEFIR_OK;
}