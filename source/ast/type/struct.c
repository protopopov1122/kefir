#include <string.h>
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_structure_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_STRUCTURE &&
        type2->tag == KEFIR_AST_TYPE_STRUCTURE, false);
    REQUIRE(type1->structure_type.complete == type2->structure_type.complete, false);
    REQUIRE((type1->structure_type.identifier == NULL && type2->structure_type.identifier == NULL) ||
        strcmp(type1->structure_type.identifier, type2->structure_type.identifier) == 0, false);
    if (type1->structure_type.complete) {
        REQUIRE(kefir_list_length(&type1->structure_type.fields) == kefir_list_length(&type2->structure_type.fields), false);
        const struct kefir_list_entry *iter1 = kefir_list_head(&type1->structure_type.fields);
        const struct kefir_list_entry *iter2 = kefir_list_head(&type2->structure_type.fields);
        for (; iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field1,
                iter1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field2,
                iter2->value);
            REQUIRE((field1->identifier == NULL && field2->identifier == NULL) ||
                strcmp(field1->identifier, field2->identifier) == 0, false);
            REQUIRE(field1->alignment->value == field2->alignment->value, false);
            REQUIRE((!field1->bitfield && !field2->bitfield) ||
                (field1->bitwidth == field2->bitwidth), false);
            REQUIRE(KEFIR_AST_TYPE_SAME(field1->type, field2->type), false);
        }
    }
    return true;
}

static kefir_bool_t compatible_structure_types(const struct kefir_ast_type_traits *type_traits,
                                             const struct kefir_ast_type *type1,
                                             const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_STRUCTURE &&
        type2->tag == KEFIR_AST_TYPE_STRUCTURE, false);
    REQUIRE((type1->structure_type.identifier == NULL && type2->structure_type.identifier == NULL) ||
        strcmp(type1->structure_type.identifier, type2->structure_type.identifier) == 0, false);
    if (type1->structure_type.complete && type2->structure_type.complete) {
        REQUIRE(kefir_list_length(&type1->structure_type.fields) == kefir_list_length(&type2->structure_type.fields), false);
        const struct kefir_list_entry *iter1 = kefir_list_head(&type1->structure_type.fields);
        const struct kefir_list_entry *iter2 = kefir_list_head(&type2->structure_type.fields);
        for (; iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field1,
                iter1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field2,
                iter2->value);
            REQUIRE((field1->identifier == NULL && field2->identifier == NULL) ||
                strcmp(field1->identifier, field2->identifier) == 0, false);
            REQUIRE(field1->alignment->value == field2->alignment->value, false);
            REQUIRE((!field1->bitfield && !field2->bitfield) ||
                (field1->bitwidth == field2->bitwidth), false);
            REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, field1->type, field2->type), false);
        }
    }
    return true;
}

static kefir_bool_t same_union_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_UNION &&
        type2->tag == KEFIR_AST_TYPE_UNION, false);
    REQUIRE(type1->structure_type.complete == type2->structure_type.complete, false);
    REQUIRE((type1->structure_type.identifier == NULL && type2->structure_type.identifier == NULL) ||
        strcmp(type1->structure_type.identifier, type2->structure_type.identifier) == 0, false);
    if (type1->structure_type.complete) {
        struct kefir_hashtree_node_iterator iter;
        kefir_size_t type1_field_count = 0, type2_field_count = 0;
        for (const struct kefir_hashtree_node *node1 = kefir_hashtree_iter(&type1->structure_type.field_index, &iter);
            node1 != NULL;
            node1 = kefir_hashtree_next(&iter), type1_field_count++) {
            struct kefir_hashtree_node *node2 = NULL;
            kefir_result_t res = kefir_hashtree_at(&type1->structure_type.field_index, node1->key, &node2);
            REQUIRE(res == KEFIR_OK, false);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field1,
                node1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field2,
                node2->value);
            REQUIRE((field1->identifier == NULL && field2->identifier == NULL) ||
                strcmp(field1->identifier, field2->identifier) == 0, false);
            REQUIRE((!field1->bitfield && !field2->bitfield) ||
                (field1->bitwidth == field2->bitwidth), false);
            REQUIRE(KEFIR_AST_TYPE_SAME(field1->type, field2->type), false);
        }
        for (const struct kefir_hashtree_node *node2 = kefir_hashtree_iter(&type2->structure_type.field_index, &iter);
            node2 != NULL;
            node2 = kefir_hashtree_next(&iter), type2_field_count++) {
        }
        REQUIRE(type1_field_count == type2_field_count, false);
    }
    return true;
}

static kefir_bool_t compatible_union_types(const struct kefir_ast_type_traits *type_traits,
                                         const struct kefir_ast_type *type1,
                                         const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_UNION &&
        type2->tag == KEFIR_AST_TYPE_UNION, false);
    REQUIRE((type1->structure_type.identifier == NULL && type2->structure_type.identifier == NULL) ||
        strcmp(type1->structure_type.identifier, type2->structure_type.identifier) == 0, false);
    if (type1->structure_type.complete && type2->structure_type.complete) {
        struct kefir_hashtree_node_iterator iter;
        kefir_size_t type1_field_count = 0, type2_field_count = 0;
        for (const struct kefir_hashtree_node *node1 = kefir_hashtree_iter(&type1->structure_type.field_index, &iter);
            node1 != NULL;
            node1 = kefir_hashtree_next(&iter), type1_field_count++) {
            struct kefir_hashtree_node *node2 = NULL;
            kefir_result_t res = kefir_hashtree_at(&type1->structure_type.field_index, node1->key, &node2);
            REQUIRE(res == KEFIR_OK, false);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field1,
                node1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field2,
                node2->value);
            REQUIRE((field1->identifier == NULL && field2->identifier == NULL) ||
                strcmp(field1->identifier, field2->identifier) == 0, false);
            REQUIRE((!field1->bitfield && !field2->bitfield) ||
                (field1->bitwidth == field2->bitwidth), false);
            REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, field1->type, field2->type), false);
        }
        for (const struct kefir_hashtree_node *node2 = kefir_hashtree_iter(&type2->structure_type.field_index, &iter);
            node2 != NULL;
            node2 = kefir_hashtree_next(&iter), type2_field_count++) {
        }
        REQUIRE(type1_field_count == type2_field_count, false);
    }
    return true;
}

static kefir_result_t free_structure(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    if (type->structure_type.complete) {
        REQUIRE_OK(kefir_hashtree_free(mem, (struct kefir_hashtree *) &type->structure_type.field_index));
        REQUIRE_OK(kefir_list_free(mem, (struct kefir_list *) &type->structure_type.fields));
    }
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_incomplete_structure(struct kefir_mem *mem,
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
    type->tag = KEFIR_AST_TYPE_STRUCTURE;
    type->basic = false;
    type->ops.same = same_structure_type;
    type->ops.compatible = compatible_structure_types;
    type->ops.free = free_structure;
    type->structure_type.complete = false;
    type->structure_type.identifier = identifier;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_incomplete_union(struct kefir_mem *mem,
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
    type->tag = KEFIR_AST_TYPE_UNION;
    type->basic = false;
    type->ops.same = same_union_type;
    type->ops.compatible = compatible_union_types;
    type->ops.free = free_structure;
    type->structure_type.complete = false;
    type->structure_type.identifier = identifier;
    return type;
}

static kefir_result_t struct_field_free(struct kefir_mem *mem,
                                      struct kefir_list *list,
                                      struct kefir_list_entry *entry,
                                      void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field,
        entry->value);
    if (field->alignment != NULL) {
        REQUIRE_OK(kefir_ast_alignment_free(mem, field->alignment));
    }
    if (field->bitfield) {
        REQUIRE_OK(kefir_ast_constant_expression_free(mem, field->bitwidth));
        KEFIR_FREE(mem, field->bitwidth);
    }
    KEFIR_FREE(mem, (void *) field);
    return KEFIR_OK;
}

static kefir_result_t kefir_ast_struct_type_field_impl(struct kefir_mem *mem,
                                                   struct kefir_symbol_table *symbols,
                                                   struct kefir_ast_struct_type *struct_type,
                                                   const char *identifier,
                                                   const struct kefir_ast_type *type,
                                                   struct kefir_ast_alignment *alignment,
                                                   kefir_bool_t bitfield,
                                                   struct kefir_ast_constant_expression *bitwidth) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(struct_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST structure type"));
    REQUIRE(identifier == NULL || strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid field identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid field type"));
    if (identifier != NULL && kefir_hashtree_has(&struct_type->field_index, (kefir_hashtree_key_t) identifier)) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Duplicate structure field identifier");
    }
    struct kefir_ast_struct_field *field = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_struct_field));
    REQUIRE(field != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for aggregate field"));
    if (symbols != NULL && identifier != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE_ELSE(identifier != NULL, {
            KEFIR_FREE(mem, field);
            return KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate field identifier");
        });
    }
    field->identifier = identifier;
    field->type = type;
    field->bitfield = bitfield;
    field->bitwidth = bitwidth;
    if (alignment != NULL) {
        field->alignment = alignment;
    } else {
        field->alignment = kefir_ast_alignment_default(mem);
        REQUIRE(field->alignment != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate default AST alignment"));
    }
    kefir_result_t res = KEFIR_OK;
    if (identifier != NULL) {
        res = kefir_hashtree_insert(mem, &struct_type->field_index,
            (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) field);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, field);
            return res;
        });
    }
    res = kefir_list_insert_after(mem, &struct_type->fields, kefir_list_tail(&struct_type->fields), field);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_delete(mem, &struct_type->field_index, (kefir_hashtree_key_t) identifier);
        KEFIR_FREE(mem, field);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_struct_type_get_field(const struct kefir_ast_struct_type *struct_type,
                                           const char *identifier,
                                           const struct kefir_ast_struct_field **field) {
    REQUIRE(struct_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST structure type"));
    REQUIRE(identifier == NULL || strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid field identifier"));
    REQUIRE(field != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST structure field pointer"));
    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&struct_type->field_index, (kefir_hashtree_key_t) identifier, &node));
    *field = (const struct kefir_ast_struct_field *) node->value;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_struct_type_field(struct kefir_mem *mem,
                                       struct kefir_symbol_table *symbols,
                                       struct kefir_ast_struct_type *struct_type,
                                       const char *identifier,
                                       const struct kefir_ast_type *type,
                                       struct kefir_ast_alignment *alignment) {
    return kefir_ast_struct_type_field_impl(mem, symbols, struct_type, identifier, type, alignment, false, NULL);
}

kefir_result_t kefir_ast_struct_type_bitfield(struct kefir_mem *mem,
                                          struct kefir_symbol_table *symbols,
                                          struct kefir_ast_struct_type *struct_type,
                                          const char *identifier,
                                          const struct kefir_ast_type *type,
                                          struct kefir_ast_alignment *alignment,
                                          struct kefir_ast_constant_expression *bitwidth) {
    REQUIRE(bitwidth != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant exression bitwidth"));
    return kefir_ast_struct_type_field_impl(mem, symbols, struct_type, identifier, type, alignment, true, bitwidth);
}

const struct kefir_ast_type *kefir_ast_type_structure(struct kefir_mem *mem,
                                                  struct kefir_ast_type_storage *type_storage,
                                                  const char *identifier,
                                                  struct kefir_ast_struct_type **struct_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(struct_type != NULL, NULL);
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
    type->tag = KEFIR_AST_TYPE_STRUCTURE;
    type->basic = false;
    type->ops.same = same_structure_type;
    type->ops.compatible = compatible_structure_types;
    type->ops.free = free_structure;
    type->structure_type.complete = true;
    type->structure_type.identifier = identifier;
    kefir_result_t res = kefir_hashtree_init(&type->structure_type.field_index, &kefir_hashtree_str_ops);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_init(&type->structure_type.fields);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->structure_type.field_index);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_on_remove(&type->structure_type.fields, struct_field_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->structure_type.field_index);
        kefir_list_free(mem, &type->structure_type.fields);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    *struct_type = &type->structure_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_union(struct kefir_mem *mem,
                                              struct kefir_ast_type_storage *type_storage,
                                              const char *identifier,
                                              struct kefir_ast_struct_type **union_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(union_type != NULL, NULL);
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
    type->tag = KEFIR_AST_TYPE_UNION;
    type->basic = false;
    type->ops.same = same_union_type;
    type->ops.compatible = compatible_union_types;
    type->ops.free = free_structure;
    type->structure_type.complete = true;
    type->structure_type.identifier = identifier;
    kefir_result_t res = kefir_hashtree_init(&type->structure_type.field_index, &kefir_hashtree_str_ops);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_init(&type->structure_type.fields);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->structure_type.field_index);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_on_remove(&type->structure_type.fields, struct_field_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->structure_type.field_index);
        kefir_list_free(mem, &type->structure_type.fields);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    *union_type = &type->structure_type;
    return type;
}