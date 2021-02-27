#include "kefir/ast/type.h"
#include "kefir/ast/node_base.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_array_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ARRAY &&
        type2->tag == KEFIR_AST_TYPE_ARRAY, false);
    REQUIRE(type1->array_type.boundary == type2->array_type.boundary, false);
    switch (type1->array_type.boundary) {
        case KEFIR_AST_ARRAY_UNBOUNDED:
        case KEFIR_AST_ARRAY_VLA:
        case KEFIR_AST_ARRAY_VLA_STATIC:
            break;

        case KEFIR_AST_ARRAY_BOUNDED:
        case KEFIR_AST_ARRAY_BOUNDED_STATIC:
            REQUIRE(type1->array_type.length == type2->array_type.length, false);
            break;
    }
    return KEFIR_AST_TYPE_SAME(type1->array_type.element_type, type2->array_type.element_type);
}

static kefir_bool_t compatbile_array_types(const struct kefir_ast_type_traits *type_traits,
                                         const struct kefir_ast_type *type1,
                                         const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ARRAY &&
        type2->tag == KEFIR_AST_TYPE_ARRAY, false);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1->array_type.element_type, type2->array_type.element_type),
        false);
    if ((type1->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED ||
        type1->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC) &&
        (type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED ||
        type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC)) {
        REQUIRE(type1->array_type.length == type2->array_type.length, false);
    }
    return true;
}

static kefir_result_t free_array(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    if ((type->array_type.boundary == KEFIR_AST_ARRAY_VLA ||
        type->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC) &&
        type->array_type.vla_length != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, type->array_type.vla_length));
    }
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

static struct kefir_ast_type *kefir_ast_type_array_impl(struct kefir_mem *mem,
                                             struct kefir_ast_type_storage *type_storage,
                                             const struct kefir_ast_type *element_type,
                                             const struct kefir_ast_type_qualification *qualification,
                                             struct kefir_ast_array_type **array_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(element_type != NULL, NULL);
    REQUIRE(array_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_storage != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &type_storage->types, kefir_list_tail(&type_storage->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_ARRAY;
    type->basic = false;
    type->ops.same = same_array_type;
    type->ops.compatible = compatbile_array_types;
    type->ops.free = free_array;
    type->array_type.element_type = element_type;
    if (qualification != NULL) {
        type->array_type.qualifications = *qualification;
    } else {
        type->array_type.qualifications = (const struct kefir_ast_type_qualification){false};
    }
    *array_type = &type->array_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_unbounded_array(struct kefir_mem *mem,
                                                        struct kefir_ast_type_storage *type_storage,
                                                        const struct kefir_ast_type *element_type,
                                                        const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_storage, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_UNBOUNDED;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_array(struct kefir_mem *mem,
                                              struct kefir_ast_type_storage *type_storage,
                                              const struct kefir_ast_type *element_type,
                                              kefir_size_t length,
                                              const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_storage, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_BOUNDED;
    array_type->length = length;
    return type;
}
                                              
const struct kefir_ast_type *kefir_ast_type_array_static(struct kefir_mem *mem,
                                                     struct kefir_ast_type_storage *type_storage,
                                                     const struct kefir_ast_type *element_type,
                                                     kefir_size_t length,
                                                     const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_storage, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_BOUNDED_STATIC;
    array_type->length = length;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_vlen_array(struct kefir_mem *mem,
                                                   struct kefir_ast_type_storage *type_storage,
                                                   const struct kefir_ast_type *element_type,
                                                   struct kefir_ast_node_base *length,
                                                   const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_storage, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_VLA;
    array_type->vla_length = length;
    return type;
}
                                              
const struct kefir_ast_type *kefir_ast_type_vlen_array_static(struct kefir_mem *mem,
                                                          struct kefir_ast_type_storage *type_storage,
                                                          const struct kefir_ast_type *element_type,
                                                          struct kefir_ast_node_base *length,
                                                          const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_storage, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_VLA_STATIC;
    array_type->vla_length = length;
    return type;
}
