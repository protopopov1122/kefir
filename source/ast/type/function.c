#include <string.h>
#include "kefir/ast/type.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_size_t kefir_ast_type_function_parameter_count(const struct kefir_ast_function_type *function_type) {
    REQUIRE(function_type != NULL, 0);
    return kefir_list_length(&function_type->parameters);
}

kefir_result_t kefir_ast_type_function_get_parameter(const struct kefir_ast_function_type *function_type,
                                                     kefir_size_t index,
                                                     const struct kefir_ast_function_type_parameter **parameter) {
    REQUIRE(function_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST finction type"));
    REQUIRE(parameter != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type parameter pointer"));
    struct kefir_list_entry *entry = kefir_list_at(&function_type->parameters, index);
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Expected valid AST function type parameter index"));
    *parameter = (const struct kefir_ast_function_type_parameter *) entry->value;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_function_parameter(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                 struct kefir_ast_function_type *function_type, const char *identifier,
                                                 const struct kefir_ast_type *type,
                                                 const kefir_ast_scoped_identifier_storage_t *storage) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_bundle != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type storage"));
    REQUIRE(function_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST finction type"));
    switch (function_type->mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS:
            REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function parameter type"));
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS:
            REQUIRE(identifier != NULL && strlen(identifier) > 0,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function parameter identifier"));
            REQUIRE(type == NULL,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected empty AST function parameter identifier type"));
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY:
            if (type != NULL) {
                function_type->mode = KEFIR_AST_FUNCTION_TYPE_PARAMETERS;
            } else if (identifier != NULL && strlen(identifier) > 0) {
                function_type->mode = KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS;
            } else {
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                       "Expected either valid AST function parameter type or identifier");
            }
            break;
    }
    if (identifier != NULL && strlen(identifier) > 0 &&
        kefir_hashtree_has(&function_type->parameter_index, (kefir_hashtree_key_t) identifier)) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Duplicate function parameter identifier");
    }
    struct kefir_ast_function_type_parameter *param =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_function_type_parameter));
    REQUIRE(param != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for function parameter"));
    if (identifier != NULL && strlen(identifier) > 0) {
        identifier = kefir_symbol_table_insert(mem, type_bundle->symbols, identifier, NULL);
        REQUIRE_ELSE(identifier != NULL, {
            KEFIR_FREE(mem, param);
            return KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate parameter identifier");
        });
    }
    param->identifier = identifier;
    param->type = type;
    if (type) {
        param->adjusted_type = kefir_ast_type_conv_adjust_function_parameter(mem, type_bundle, type);
        REQUIRE(param->adjusted_type != NULL,
                KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to adjust AST function parameter type"));
    } else {
        param->adjusted_type = NULL;
    }
    if (storage == NULL) {
        KEFIR_OPTIONAL_SET_EMPTY(&param->storage);
    } else {
        KEFIR_OPTIONAL_SET_VALUE(&param->storage, *storage);
    }
    kefir_result_t res = KEFIR_OK;
    if (identifier != NULL && strlen(identifier) > 0) {
        res = kefir_hashtree_insert(mem, &function_type->parameter_index, (kefir_hashtree_key_t) identifier,
                                    (kefir_hashtree_value_t) param);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, param);
            return res;
        });
    }
    res = kefir_list_insert_after(mem, &function_type->parameters, kefir_list_tail(&function_type->parameters), param);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_delete(mem, &function_type->parameter_index, (kefir_hashtree_key_t) identifier);
        KEFIR_FREE(mem, param);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_function_ellipsis(struct kefir_ast_function_type *function_type, kefir_bool_t ellipsis) {
    REQUIRE(function_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    function_type->ellipsis = ellipsis;
    return KEFIR_OK;
}

static kefir_bool_t same_function_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_FUNCTION && type2->tag == KEFIR_AST_TYPE_FUNCTION, false);
    REQUIRE(KEFIR_AST_TYPE_SAME(type1->function_type.return_type, type2->function_type.return_type), false);
    REQUIRE((type1->function_type.identifier == NULL && type2->function_type.identifier == NULL) ||
                (type1->function_type.identifier != NULL && type2->function_type.identifier != NULL &&
                 strcmp(type1->function_type.identifier, type2->function_type.identifier) == 0),
            false);
    REQUIRE(type1->function_type.mode == type2->function_type.mode, false);
    REQUIRE(kefir_list_length(&type1->function_type.parameters) == kefir_list_length(&type2->function_type.parameters),
            false);
    const struct kefir_list_entry *iter1 = kefir_list_head(&type1->function_type.parameters);
    const struct kefir_list_entry *iter2 = kefir_list_head(&type2->function_type.parameters);
    for (; iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_function_type_parameter *, param1, iter1->value);
        ASSIGN_DECL_CAST(const struct kefir_ast_function_type_parameter *, param2, iter2->value);
        REQUIRE((param1->identifier == NULL && param2->identifier == NULL) ||
                    (param1->identifier != NULL && param2->identifier != NULL &&
                     strcmp(param1->identifier, param2->identifier) == 0),
                false);
        REQUIRE((param1->type == NULL && param2->type == NULL) ||
                    (param1->type != NULL && param2->type != NULL && KEFIR_AST_TYPE_SAME(param1->type, param2->type)),
                false);
        REQUIRE((KEFIR_OPTIONAL_EMPTY(&param1->storage) && KEFIR_OPTIONAL_EMPTY(&param2->storage)) ||
                    (!KEFIR_OPTIONAL_EMPTY(&param1->storage) && !KEFIR_OPTIONAL_EMPTY(&param2->storage) &&
                     *KEFIR_OPTIONAL_VALUE(&param1->storage) == *KEFIR_OPTIONAL_VALUE(&param2->storage)),
                false);
    }
    REQUIRE(type1->function_type.ellipsis == type2->function_type.ellipsis, false);
    return true;
}

static kefir_bool_t parameter_list_unaffected_by_promotions(const struct kefir_ast_type_traits *type_traits,
                                                            const struct kefir_ast_type *type) {
    const struct kefir_list_entry *iter = NULL;
    for (iter = kefir_list_head(&type->function_type.parameters); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);
        const struct kefir_ast_type *unqualified = kefir_ast_unqualified_type(param->adjusted_type);
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, unqualified,
                                          kefir_ast_type_function_default_argument_promotion(type_traits, unqualified)),
                false);
    }
    return true;
}

static kefir_bool_t compatible_function_types(const struct kefir_ast_type_traits *type_traits,
                                              const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, kefir_ast_unqualified_type(type1->function_type.return_type),
                                      kefir_ast_unqualified_type(type2->function_type.return_type)),
            false);
    if (type1->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS &&
        type2->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        REQUIRE(
            kefir_list_length(&type1->function_type.parameters) == kefir_list_length(&type2->function_type.parameters),
            false);
        REQUIRE(type1->function_type.ellipsis == type2->function_type.ellipsis, false);
        const struct kefir_list_entry *iter1 = NULL;
        const struct kefir_list_entry *iter2 = NULL;
        for (iter1 = kefir_list_head(&type1->function_type.parameters),
            iter2 = kefir_list_head(&type2->function_type.parameters);
             iter1 != NULL && iter2 != NULL; kefir_list_next(&iter1), kefir_list_next(&iter2)) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param1, iter1->value);
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param2, iter2->value);
            const struct kefir_ast_type *unqualified1 = kefir_ast_unqualified_type(param1->adjusted_type);
            const struct kefir_ast_type *unqualified2 = kefir_ast_unqualified_type(param2->adjusted_type);
            REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, unqualified1, unqualified2), false);
        }
        REQUIRE(iter1 == NULL && iter2 == NULL, false);
        return true;
    } else if (type1->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        REQUIRE(!type1->function_type.ellipsis, false);
        return parameter_list_unaffected_by_promotions(type_traits, type1);
    } else if (type2->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        REQUIRE(!type1->function_type.ellipsis, false);
        return parameter_list_unaffected_by_promotions(type_traits, type2);
    } else {
        return true;
    }
}

const struct kefir_ast_type *composite_function_types(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                      const struct kefir_ast_type_traits *type_traits,
                                                      const struct kefir_ast_type *type1,
                                                      const struct kefir_ast_type *type2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_traits != NULL, NULL);
    REQUIRE(type1 != NULL, NULL);
    REQUIRE(type2 != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, type2), NULL);
    if (type1->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS &&
        type2->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        const struct kefir_ast_type *composite_return_type = KEFIR_AST_TYPE_COMPOSITE(
            mem, type_bundle, type_traits, type1->function_type.return_type, type2->function_type.return_type);
        REQUIRE(composite_return_type != NULL, NULL);
        struct kefir_ast_function_type *composite_function = NULL;
        const struct kefir_ast_type *composite_type =
            kefir_ast_type_function(mem, type_bundle, composite_return_type, "", &composite_function);
        REQUIRE(composite_type != NULL, NULL);
        for (kefir_size_t i = 0; i < kefir_ast_type_function_parameter_count(&type1->function_type); i++) {
            const struct kefir_ast_function_type_parameter *param1 = NULL;
            REQUIRE(kefir_ast_type_function_get_parameter(&type1->function_type, i, &param1) == KEFIR_OK, NULL);
            const struct kefir_ast_function_type_parameter *param2 = NULL;
            REQUIRE(kefir_ast_type_function_get_parameter(&type2->function_type, i, &param2) == KEFIR_OK, NULL);
            const struct kefir_ast_type *composite_parameter = KEFIR_AST_TYPE_COMPOSITE(
                mem, type_bundle, type_traits, kefir_ast_unqualified_type(param1->adjusted_type),
                kefir_ast_unqualified_type(param2->adjusted_type));
            REQUIRE(composite_parameter != NULL, NULL);
            REQUIRE(kefir_ast_type_function_parameter(mem, type_bundle, composite_function, NULL, composite_parameter,
                                                      NULL) == KEFIR_OK,
                    NULL);
        }
        return composite_type;
    } else if (type2->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        return type2;
    } else {
        return type1;
    }
}

static kefir_result_t free_function_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE_OK(kefir_hashtree_free(mem, (struct kefir_hashtree *) &type->function_type.parameter_index));
    REQUIRE_OK(kefir_list_free(mem, (struct kefir_list *) &type->function_type.parameters));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

static kefir_result_t function_parameter_free(struct kefir_mem *mem, struct kefir_list *list,
                                              struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    ASSIGN_DECL_CAST(const struct kefir_ast_function_type_parameter *, param, entry->value);
    KEFIR_FREE(mem, (void *) param);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_function(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                     const struct kefir_ast_type *return_type, const char *identifier,
                                                     struct kefir_ast_function_type **function_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(return_type != NULL, NULL);
    REQUIRE(function_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_bundle != NULL) {
        if (identifier != NULL) {
            identifier = kefir_symbol_table_insert(mem, type_bundle->symbols, identifier, NULL);
            REQUIRE_ELSE(identifier != NULL, {
                KEFIR_FREE(mem, type);
                return NULL;
            });
        }
        kefir_result_t res =
            kefir_list_insert_after(mem, &type_bundle->types, kefir_list_tail(&type_bundle->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_FUNCTION;
    type->basic = false;
    type->ops.same = same_function_type;
    type->ops.compatible = compatible_function_types;
    type->ops.composite = composite_function_types;
    type->ops.free = free_function_type;
    type->function_type.return_type = return_type;
    type->function_type.identifier = identifier;
    type->function_type.mode = KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY;
    type->function_type.ellipsis = false;
    kefir_result_t res = kefir_hashtree_init(&type->function_type.parameter_index, &kefir_hashtree_str_ops);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_init(&type->function_type.parameters);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->function_type.parameter_index);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    res = kefir_list_on_remove(&type->function_type.parameters, function_parameter_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &type->function_type.parameter_index);
        kefir_list_free(mem, &type->function_type.parameters);
        KEFIR_FREE(mem, type);
        return NULL;
    });
    *function_type = &type->function_type;
    return type;
}
