#include <string.h>
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_enumeration_get(const struct kefir_ast_enum_type *enum_type,
                                     const char *identifier,
                                     const struct kefir_ast_constant_expression **value) {
    REQUIRE(enum_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST enum type"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST enum constant identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid enumerator constant value pointer"));
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&enum_type->enumerator_index, (kefir_hashtree_key_t) identifier, &node));
    ASSIGN_DECL_CAST(struct kefir_ast_enum_enumerator *, constant,
        node->value);
    *value = constant->value;
    return KEFIR_OK;
}

static kefir_bool_t same_enumeration_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ENUMERATION &&
        type2->tag == KEFIR_AST_TYPE_ENUMERATION, false);
    REQUIRE(type1->enumeration_type.complete == type2->enumeration_type.complete, false);
    REQUIRE((type1->enumeration_type.identifier == NULL && type2->enumeration_type.identifier == NULL) ||
        strcmp(type1->enumeration_type.identifier, type2->enumeration_type.identifier) == 0, false);
    if (type1->enumeration_type.complete) {
        REQUIRE(kefir_list_length(&type1->enumeration_type.enumerators) == kefir_list_length(&type2->enumeration_type.enumerators), false);
        const struct kefir_list_entry *iter1 = kefir_list_head(&type1->enumeration_type.enumerators);
        const struct kefir_list_entry *iter2 = kefir_list_head(&type2->enumeration_type.enumerators);
        for (; iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
            ASSIGN_DECL_CAST(const struct kefir_ast_enum_enumerator *, enum1,
                iter1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_enum_enumerator *, enum2,
                iter2->value);
            REQUIRE(strcmp(enum1->identifier, enum2->identifier) == 0, false);
            REQUIRE(enum1->value->value == enum2->value->value, false);
        }
    }
    return true;
}

static kefir_bool_t compatible_enumeration_types(const struct kefir_ast_type_traits *type_traits,
                                               const struct kefir_ast_type *type1,
                                               const struct kefir_ast_type *type2) {
    UNUSED(type_traits);
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    if (type1->tag == KEFIR_AST_TYPE_ENUMERATION &&
        KEFIR_AST_TYPE_SAME(kefir_ast_enumeration_underlying_type(&type1->enumeration_type), type2)) {
        return true;
    } else if (type2->tag == KEFIR_AST_TYPE_ENUMERATION &&
        KEFIR_AST_TYPE_SAME(type1, kefir_ast_enumeration_underlying_type(&type2->enumeration_type))) {
        return true;
    }
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ENUMERATION &&
        type2->tag == KEFIR_AST_TYPE_ENUMERATION, false);
    REQUIRE((type1->enumeration_type.identifier == NULL && type2->enumeration_type.identifier == NULL) ||
        strcmp(type1->enumeration_type.identifier, type2->enumeration_type.identifier) == 0, false);
    if (type1->enumeration_type.complete && type2->enumeration_type.complete) {
        REQUIRE(kefir_list_length(&type1->enumeration_type.enumerators) ==
            kefir_list_length(&type2->enumeration_type.enumerators), false);
        const struct kefir_list_entry *iter1 = kefir_list_head(&type1->enumeration_type.enumerators);
        const struct kefir_list_entry *iter2 = kefir_list_head(&type2->enumeration_type.enumerators);
        for (; iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
            ASSIGN_DECL_CAST(const struct kefir_ast_enum_enumerator *, enum1,
                iter1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_enum_enumerator *, enum2,
                iter2->value);
            REQUIRE(strcmp(enum1->identifier, enum2->identifier) == 0, false);
            REQUIRE(enum1->value->value == enum2->value->value, false);
        }
    }
    return true;
}

static kefir_result_t free_enumeration_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    if (type->enumeration_type.complete) {
        REQUIRE_OK(kefir_hashtree_free(mem, (struct kefir_hashtree *) &type->enumeration_type.enumerator_index));
        REQUIRE_OK(kefir_list_free(mem, (struct kefir_list *) &type->enumeration_type.enumerators));
    }
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_incomplete_enumeration(struct kefir_mem *mem,
                                                               struct kefir_ast_type_storage *type_storage,
                                                               const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_storage != NULL) {
        identifier = kefir_symbol_table_insert(mem, type_storage->symbols, identifier, NULL);
        REQUIRE_ELSE(identifier != NULL, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
        kefir_result_t res = kefir_list_insert_after(mem, &type_storage->types, kefir_list_tail(&type_storage->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_ENUMERATION;
    type->basic = false;
    type->ops.same = same_enumeration_type;
    type->ops.compatible = compatible_enumeration_types;
    type->ops.free = free_enumeration_type;
    type->structure_type.complete = false;
    type->structure_type.identifier = identifier;
    return type;
}

static kefir_result_t enumeration_free(struct kefir_mem *mem,
                                     struct kefir_list *list,
                                     struct kefir_list_entry *entry,
                                     void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    ASSIGN_DECL_CAST(const struct kefir_ast_enum_enumerator *, enumerator,
        entry->value);
    REQUIRE_OK(kefir_ast_constant_expression_free(mem, enumerator->value));
    KEFIR_FREE(mem, enumerator->value);
    KEFIR_FREE(mem, (void *) enumerator);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_enumeration_type_constant(struct kefir_mem *mem,
                                               struct kefir_symbol_table *symbols,
                                               struct kefir_ast_enum_type *enum_type,
                                               const char *identifier,
                                               struct kefir_ast_constant_expression *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(enum_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST enum type"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid constant identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));
    if (kefir_hashtree_has(&enum_type->enumerator_index, (kefir_hashtree_key_t) identifier)) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Duplicate enumerate constant identifier");
    }
    struct kefir_ast_enum_enumerator *enum_constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_enum_enumerator));
    REQUIRE(enum_constant != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for enumeration constant"));
    if (symbols != NULL && identifier != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE_ELSE(identifier != NULL, {
            KEFIR_FREE(mem, enum_constant);
            return KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate enumeration constant");
        });
    }
    enum_constant->identifier = identifier;
    enum_constant->value = value;
    kefir_result_t res = kefir_hashtree_insert(mem, &enum_type->enumerator_index,
        (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) enum_constant);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, enum_constant);
        return res;
    });
    res = kefir_list_insert_after(mem, &enum_type->enumerators, kefir_list_tail(&enum_type->enumerators), enum_constant);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_delete(mem, &enum_type->enumerator_index, (kefir_hashtree_key_t) identifier);
        KEFIR_FREE(mem, enum_constant);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_enumeration_type_constant_auto(struct kefir_mem *mem,
                                                    struct kefir_symbol_table *symbols,
                                                    struct kefir_ast_enum_type *enum_type,
                                                    const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(enum_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST enum type"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid constant identifier"));
    struct kefir_list_entry *tail = kefir_list_tail(&enum_type->enumerators);
    struct kefir_ast_constant_expression *const_expr = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant_expression));
    REQUIRE(const_expr != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST constant expression"));
    const_expr->value = 0;
    const_expr->expression = NULL;
    if (tail != NULL) {
        ASSIGN_DECL_CAST(struct kefir_ast_enum_enumerator *, prev,
            tail->value);
        const_expr->value = prev->value->value + 1;
    }
    return kefir_ast_enumeration_type_constant(mem, symbols, enum_type, identifier, const_expr);
}

const struct kefir_ast_type *kefir_ast_enumeration_underlying_type(const struct kefir_ast_enum_type *enumeration) {
    // TODO: Proper underlying type detection
    REQUIRE(enumeration != NULL, NULL);
    return kefir_ast_type_signed_int();
}

const struct kefir_ast_type *kefir_ast_type_enumeration(struct kefir_mem *mem,
                                                   struct kefir_ast_type_storage *type_storage,
                                                   const char *identifier,
                                                   struct kefir_ast_enum_type **enum_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(enum_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_storage != NULL) {
        if (identifier != NULL) {
            identifier = kefir_symbol_table_insert(mem, type_storage->symbols, identifier, NULL);
            REQUIRE_ELSE(identifier != NULL, {
                KEFIR_FREE(mem, type);
                return NULL;
            });
        }
        kefir_result_t res = kefir_list_insert_after(mem, &type_storage->types, kefir_list_tail(&type_storage->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_ENUMERATION;
    type->basic = false;
    type->ops.same = same_enumeration_type;
    type->ops.compatible = compatible_enumeration_types;
    type->ops.free = free_enumeration_type;
    type->enumeration_type.complete = true;
    type->enumeration_type.identifier = identifier;
    kefir_result_t res = kefir_hashtree_init(&type->enumeration_type.enumerator_index, &kefir_hashtree_str_ops);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_init(&type->enumeration_type.enumerators);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->enumeration_type.enumerator_index);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_on_remove(&type->enumeration_type.enumerators, enumeration_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->enumeration_type.enumerator_index);
        kefir_list_free(mem, &type->enumeration_type.enumerators);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    *enum_type = &type->enumeration_type;
    return type;
}
