#include <string.h>
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_basic_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    return type1->tag == type2->tag;
}

static kefir_result_t free_nothing(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    UNUSED(mem);
    UNUSED(type);
    return KEFIR_OK;
}

static const struct kefir_ast_type SCALAR_VOID = {
    .tag = KEFIR_AST_TYPE_VOID,
    .basic = false,
    .ops = {
        .same = same_basic_type,
        .free = free_nothing
    }
};

const struct kefir_ast_type *kefir_ast_type_void() {
    return &SCALAR_VOID;
}

#define SCALAR_TYPE(id, _tag, _signedness, _rank) \
static const struct kefir_ast_type SCALAR_##id = { \
    .tag = _tag, \
    .basic = true, \
    .ops = { \
        .same = same_basic_type, \
        .free = free_nothing \
    }, \
    .basic_traits = { \
        .signedness = _signedness, \
        .rank = _rank \
    } \
}; \
\
const struct kefir_ast_type *kefir_ast_type_##id() { \
    return &SCALAR_##id; \
}

SCALAR_TYPE(bool, KEFIR_AST_TYPE_SCALAR_BOOL, false, 0)
SCALAR_TYPE(unsigned_char, KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR, false, 1)
SCALAR_TYPE(signed_char, KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR, true, 1)
SCALAR_TYPE(unsigned_short, KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT, false, 2)
SCALAR_TYPE(signed_short, KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT, true, 2)
SCALAR_TYPE(unsigned_int, KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT, false, 3)
SCALAR_TYPE(signed_int, KEFIR_AST_TYPE_SCALAR_SIGNED_INT, true, 3)
SCALAR_TYPE(unsigned_long, KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG, false, 4)
SCALAR_TYPE(signed_long, KEFIR_AST_TYPE_SCALAR_SIGNED_LONG, true, 4)
SCALAR_TYPE(unsigned_long_long, KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG, false, 4)
SCALAR_TYPE(signed_long_long, KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG, true, 4)
SCALAR_TYPE(float, KEFIR_AST_TYPE_SCALAR_FLOAT, false, 5)
SCALAR_TYPE(double, KEFIR_AST_TYPE_SCALAR_DOUBLE, false, 6)

#undef SCALAR_TYPE

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
    return type1->qualified.constant == type2->qualified.constant &&
        type1->qualified.restricted == type2->qualified.restricted &&
        type1->qualified.volatile_type == type2->qualified.volatile_type &&
        KEFIR_AST_TYPE_SAME(type1->qualified.type, type2->qualified.type);
}

const struct kefir_ast_type *kefir_ast_type_qualified(struct kefir_mem *mem,
                                                 struct kefir_ast_type_repository *repo,
                                                 const struct kefir_ast_type *base_type,
                                                 kefir_bool_t constant,
                                                 kefir_bool_t restricted,
                                                 kefir_bool_t volatile_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_QUALIFIED;
    type->basic = false;
    type->ops.same = same_qualified_type;
    type->ops.free = free_qualified_type;
    type->qualified.constant = constant;
    type->qualified.restricted = restricted;
    type->qualified.volatile_type = volatile_type;
    type->qualified.type = base_type;
    return type;
}

static kefir_bool_t same_pointer_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER &&
        type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER, false);
    return KEFIR_AST_TYPE_SAME(type1->pointer_to, type2->pointer_to);
}

static kefir_result_t free_pointer_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_pointer(struct kefir_mem *mem,
                                                struct kefir_ast_type_repository *repo,
                                                const struct kefir_ast_type *base_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_SCALAR_POINTER;
    type->basic = false;
    type->ops.same = same_pointer_type;
    type->ops.free = free_pointer_type;
    type->pointer_to = base_type;
    return type;
}

static kefir_bool_t same_structure_union_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE((type1->tag == KEFIR_AST_TYPE_STRUCTURE &&
        type2->tag == KEFIR_AST_TYPE_STRUCTURE) ||
        (type1->tag == KEFIR_AST_TYPE_UNION &&
        type2->tag == KEFIR_AST_TYPE_UNION), false);
    REQUIRE(type1->structure.complete == type2->structure.complete, false);
    REQUIRE((type1->structure.identifier == NULL && type2->structure.identifier == NULL) ||
        strcmp(type1->structure.identifier, type2->structure.identifier) == 0, false);
    if (type1->structure.complete) {
        REQUIRE(kefir_list_length(&type1->structure.fields) == kefir_list_length(&type2->structure.fields), false);
        const struct kefir_list_entry *iter1 = kefir_list_head(&type1->structure.fields);
        const struct kefir_list_entry *iter2 = kefir_list_head(&type2->structure.fields);
        for (; iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field1,
                iter1->value);
            ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field2,
                iter2->value);
            REQUIRE((field1->identifier == NULL && field1->identifier == NULL) ||
                strcmp(field1->identifier, field2->identifier) == 0, false);
            REQUIRE((!field1->bitfield && !field2->bitfield) ||
                (field1->bitwidth == field2->bitwidth), false);
            REQUIRE(KEFIR_AST_TYPE_SAME(field1->type, field2->type), false);
        }
    }
    return true;
}

static kefir_result_t free_structure_union(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    if (type->structure.complete) {
        REQUIRE_OK(kefir_list_free(mem, (struct kefir_list *) &type->structure.fields));
    }
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_incomplete_structure(struct kefir_mem *mem,
                                                             struct kefir_ast_type_repository *repo,
                                                             const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        identifier = kefir_symbol_table_insert(mem, &repo->symbols, identifier, NULL);
        REQUIRE_ELSE(identifier != NULL, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_STRUCTURE;
    type->basic = false;
    type->ops.same = same_structure_union_type;
    type->ops.free = free_structure_union;
    type->structure.complete = false;
    type->structure.identifier = identifier;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_incomplete_union(struct kefir_mem *mem,
                                                         struct kefir_ast_type_repository *repo,
                                                         const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        identifier = kefir_symbol_table_insert(mem, &repo->symbols, identifier, NULL);
        REQUIRE_ELSE(identifier != NULL, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_UNION;
    type->basic = false;
    type->ops.same = same_structure_union_type;
    type->ops.free = free_structure_union;
    type->structure.complete = false;
    type->structure.identifier = identifier;
    return type;
}

static kefir_result_t struct_union_field_free(struct kefir_mem *mem,
                                            struct kefir_list *list,
                                            struct kefir_list_entry *entry,
                                            void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    ASSIGN_DECL_CAST(const struct kefir_ast_struct_field *, field,
        entry->value);
    KEFIR_FREE(mem, (void *) field);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_structure(struct kefir_mem *mem,
                                                  struct kefir_ast_type_repository *repo,
                                                  const char *identifier,
                                                  struct kefir_ast_struct_type **struct_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(struct_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        if (identifier != NULL) {
            identifier = kefir_symbol_table_insert(mem, &repo->symbols, identifier, NULL);
            REQUIRE_ELSE(identifier != NULL, {
                KEFIR_FREE(mem, type);
                return NULL;
            });
        }
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_STRUCTURE;
    type->basic = false;
    type->ops.same = same_structure_union_type;
    type->ops.free = free_structure_union;
    type->structure.complete = true;
    type->structure.identifier = identifier;
    kefir_result_t res = kefir_list_init(&type->structure.fields);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_on_remove(&type->structure.fields, struct_union_field_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &type->structure.fields);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    *struct_type = &type->structure;
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_union(struct kefir_mem *mem,
                                              struct kefir_ast_type_repository *repo,
                                              const char *identifier,
                                              struct kefir_ast_struct_type **union_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(union_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        if (identifier != NULL) {
            identifier = kefir_symbol_table_insert(mem, &repo->symbols, identifier, NULL);
            REQUIRE_ELSE(identifier != NULL, {
                KEFIR_FREE(mem, type);
                return NULL;
            });
        }
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_UNION;
    type->basic = false;
    type->ops.same = same_structure_union_type;
    type->ops.free = free_structure_union;
    type->structure.complete = true;
    type->structure.identifier = identifier;
    kefir_result_t res = kefir_list_init(&type->structure.fields);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_on_remove(&type->structure.fields, struct_union_field_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &type->structure.fields);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    *union_type = &type->structure;
    return KEFIR_OK;
}

static kefir_bool_t same_array_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ARRAY &&
        type2->tag == KEFIR_AST_TYPE_ARRAY, false);
    REQUIRE((!type1->array.bounded && !type2->array.bounded) ||
        (type1->array.length == type2->array.length), false);
    return KEFIR_AST_TYPE_SAME(type1->array.element_type, type2->array.element_type);
}

static kefir_result_t free_array(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_unbounded_array(struct kefir_mem *mem,
                                                        struct kefir_ast_type_repository *repo,
                                                        const struct kefir_ast_type *element_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(element_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_ARRAY;
    type->basic = false;
    type->ops.same = same_array_type;
    type->ops.free = free_array;
    type->array.bounded = false;
    type->array.element_type = element_type;
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_array(struct kefir_mem *mem,
                                              struct kefir_ast_type_repository *repo,
                                              const struct kefir_ast_type *element_type,
                                              kefir_size_t length) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(element_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (repo != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &repo->types, kefir_list_tail(&repo->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_ARRAY;
    type->basic = false;
    type->ops.same = same_array_type;
    type->ops.free = free_array;
    type->array.bounded = true;
    type->array.element_type = element_type;
    type->array.length = length;
    return KEFIR_OK;
}

static kefir_result_t free_type_repo(struct kefir_mem *mem,
                                   struct kefir_list *list,
                                   struct kefir_list_entry *entry,
                                   void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    REQUIRE_OK(KEFIR_AST_TYPE_FREE(mem, (struct kefir_ast_type *) entry->value));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_repository_init(struct kefir_ast_type_repository *repo) {
    REQUIRE(repo != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type repository"));
    REQUIRE_OK(kefir_symbol_table_init(&repo->symbols));
    REQUIRE_OK(kefir_list_init(&repo->types));
    REQUIRE_OK(kefir_list_on_remove(&repo->types, free_type_repo, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_repository_free(struct kefir_mem *mem, struct kefir_ast_type_repository *repo) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(repo != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type repository"));
    REQUIRE_OK(kefir_list_free(mem, &repo->types));
    REQUIRE_OK(kefir_symbol_table_free(mem, &repo->symbols));
    return KEFIR_OK;
}