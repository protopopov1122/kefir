#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_qualified_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

static kefir_bool_t same_qualified_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_QUALIFIED &&
        type2->tag == KEFIR_AST_TYPE_QUALIFIED, false);
    return type1->qualified_type.qualification.constant == type2->qualified_type.qualification.constant &&
        type1->qualified_type.qualification.restricted == type2->qualified_type.qualification.restricted &&
        type1->qualified_type.qualification.volatile_type == type2->qualified_type.qualification.volatile_type &&
        KEFIR_AST_TYPE_SAME(type1->qualified_type.type, type2->qualified_type.type);
}

static kefir_bool_t compatbile_qualified_types(const struct kefir_ast_type_traits *type_traits,
                                             const struct kefir_ast_type *type1,
                                             const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    if (type1->tag == KEFIR_AST_TYPE_QUALIFIED &&
        type2->tag != KEFIR_AST_TYPE_QUALIFIED) {
        return KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&type1->qualified_type.qualification) &&
            KEFIR_AST_TYPE_COMPATIBLE(type_traits, kefir_ast_unqualified_type(type1), type2);
    }
    REQUIRE(type1->tag == KEFIR_AST_TYPE_QUALIFIED &&
        type2->tag == KEFIR_AST_TYPE_QUALIFIED, false);
    return type1->qualified_type.qualification.constant == type2->qualified_type.qualification.constant &&
        type1->qualified_type.qualification.restricted == type2->qualified_type.qualification.restricted &&
        type1->qualified_type.qualification.volatile_type == type2->qualified_type.qualification.volatile_type &&
        KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1->qualified_type.type, type2->qualified_type.type);
}

const struct kefir_ast_type *kefir_ast_type_qualified(struct kefir_mem *mem,
                                                 struct kefir_ast_type_storage *type_storage,
                                                 const struct kefir_ast_type *base_type,
                                                 struct kefir_ast_type_qualification qualification) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_storage != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &type_storage->types, kefir_list_tail(&type_storage->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    if (base_type->tag == KEFIR_AST_TYPE_QUALIFIED) {
        qualification.constant = qualification.constant || base_type->qualified_type.qualification.constant;
        qualification.restricted = qualification.restricted || base_type->qualified_type.qualification.restricted;
        qualification.volatile_type = qualification.volatile_type || base_type->qualified_type.qualification.volatile_type;
        base_type = base_type->qualified_type.type;
    }
    type->tag = KEFIR_AST_TYPE_QUALIFIED;
    type->basic = false;
    type->ops.same = same_qualified_type;
    type->ops.compatible = compatbile_qualified_types;
    type->ops.free = free_qualified_type;
    type->qualified_type.qualification = qualification;
    type->qualified_type.type = base_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_unqualified_type(const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_QUALIFIED:
            return type->qualified_type.type;

        default:
            return type;
    }
    return NULL;
}

const struct kefir_ast_type *kefir_ast_zero_unqualified_type(const struct kefir_ast_type *type) {
    if (type->tag == KEFIR_AST_TYPE_QUALIFIED &&
        KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&type->qualified_type.qualification)) {
        return type->qualified_type.type;
    } else {
        return type;
    }
}