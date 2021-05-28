#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct kefir_ast_declarator *kefir_ast_declarator_identifier(struct kefir_mem *mem,
                                                         struct kefir_symbol_table *symbols,
                                                         const char *identifier) {
    REQUIRE(mem != NULL, NULL);

    if (symbols != NULL && identifier != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_IDENTIFIER;
    decl->identifier = identifier;
    return decl;
}

struct kefir_ast_declarator *kefir_ast_declarator_pointer(struct kefir_mem *mem, struct kefir_ast_declarator *direct) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(direct != NULL, NULL);

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_POINTER;

    kefir_result_t res = kefir_ast_type_qualifier_list_init(&decl->pointer.type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });

    decl->pointer.declarator = direct;
    return decl;
}

struct kefir_ast_declarator *kefir_ast_declarator_array(struct kefir_mem *mem,
                                                    kefir_ast_declarator_array_type_t type,
                                                    struct kefir_ast_node_base *length,
                                                    struct kefir_ast_declarator *direct) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(direct != NULL, NULL);
    if (type == KEFIR_AST_DECLARATOR_ARRAY_BOUNDED) {
        REQUIRE(length != NULL, NULL);
    } else {
        REQUIRE(length == NULL, NULL);
    }

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_ARRAY;

    kefir_result_t res = kefir_ast_type_qualifier_list_init(&decl->array.type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    
    decl->array.type = type;
    decl->array.length = length;
    decl->array.static_array = false;
    decl->array.declarator = direct;
    return decl;
}

static kefir_result_t free_function_param(struct kefir_mem *mem,
                                        struct kefir_list *list,
                                        struct kefir_list_entry *entry,
                                        void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param,
        entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, param));
    return KEFIR_OK;
}

struct kefir_ast_declarator *kefir_ast_declarator_function(struct kefir_mem *mem, struct kefir_ast_declarator *direct) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(direct != NULL, NULL);

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_FUNCTION;

    kefir_result_t res = kefir_list_init(&decl->function.parameters);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });

    res = kefir_list_on_remove(&decl->function.parameters, free_function_param, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &decl->function.parameters);
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    
    decl->function.ellipsis = false;
    decl->function.declarator = direct;
    return decl;
}

struct kefir_ast_declarator *kefir_ast_declarator_clone(struct kefir_mem *mem,
                                                    const struct kefir_ast_declarator *declarator) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(declarator != NULL, NULL);

    switch (declarator->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            return kefir_ast_declarator_identifier(mem, NULL, declarator->identifier);
        
        case KEFIR_AST_DECLARATOR_POINTER: {
            struct kefir_ast_declarator *decl = kefir_ast_declarator_pointer(mem,
                kefir_ast_declarator_clone(mem, declarator->pointer.declarator));
            REQUIRE(decl != NULL, NULL);

            kefir_result_t res = kefir_ast_type_qualifier_list_clone(mem, &decl->pointer.type_qualifiers,
                &declarator->pointer.type_qualifiers);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_declarator_free(mem, decl);
                return NULL;
            });
            return decl;
        } 
        
        case KEFIR_AST_DECLARATOR_ARRAY: {
            struct kefir_ast_declarator *decl = kefir_ast_declarator_array(mem, declarator->array.type,
                KEFIR_AST_NODE_CLONE(mem, declarator->array.length),
               kefir_ast_declarator_clone(mem, declarator->array.declarator));
            REQUIRE(decl != NULL, NULL);

            decl->array.static_array = declarator->array.static_array;
            kefir_result_t res = kefir_ast_type_qualifier_list_clone(mem, &decl->array.type_qualifiers,
                &declarator->array.type_qualifiers);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_declarator_free(mem, decl);
                return NULL;
            });
            return decl;
        }
        
        case KEFIR_AST_DECLARATOR_FUNCTION: {
            struct kefir_ast_declarator *decl = kefir_ast_declarator_function(mem,
                kefir_ast_declarator_clone(mem, declarator->function.declarator));
            REQUIRE(decl != NULL, NULL);
            
            decl->function.ellipsis = declarator->function.ellipsis;
            for (const struct kefir_list_entry *iter = kefir_list_head(&declarator->function.parameters);
                iter != NULL;
                kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param,
                    iter->value);
                struct kefir_ast_node_base *param_clone = KEFIR_AST_NODE_CLONE(mem, param);
                REQUIRE_ELSE(param_clone != NULL, {
                    kefir_ast_declarator_free(mem, decl);
                    return NULL;
                });

                kefir_result_t res = kefir_list_insert_after(mem, &decl->function.parameters, kefir_list_tail(&decl->function.parameters),
                    param_clone);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    KEFIR_AST_NODE_FREE(mem, param_clone);
                    kefir_ast_declarator_free(mem, decl);
                    return NULL;
                });
            }
            return decl;
        }
    }
    return NULL;
}

kefir_result_t kefir_ast_declarator_free(struct kefir_mem *mem, struct kefir_ast_declarator *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator"));

    switch (decl->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            decl->identifier = NULL;
            break;

        case KEFIR_AST_DECLARATOR_POINTER:
            REQUIRE_OK(kefir_ast_type_qualifier_list_free(mem, &decl->pointer.type_qualifiers));
            REQUIRE_OK(kefir_ast_declarator_free(mem, decl->pointer.declarator));
            decl->pointer.declarator = NULL;
            break;

        case KEFIR_AST_DECLARATOR_ARRAY:
            REQUIRE_OK(kefir_ast_type_qualifier_list_free(mem, &decl->array.type_qualifiers));
            if (decl->array.length != NULL) {
                REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, decl->array.length));
                decl->array.length = NULL;
            }
            decl->array.static_array = false;
            REQUIRE_OK(kefir_ast_declarator_free(mem, decl->array.declarator));
            decl->array.declarator = NULL;
            break;

        case KEFIR_AST_DECLARATOR_FUNCTION:
            REQUIRE_OK(kefir_list_free(mem, &decl->function.parameters));
            REQUIRE_OK(kefir_ast_declarator_free(mem, decl->function.declarator));
            decl->function.declarator = NULL;
            break;

    }
    KEFIR_FREE(mem, decl);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_is_abstract(struct kefir_ast_declarator *decl, kefir_bool_t *result) {
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to boolean"));

    switch (decl->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            *result = decl->identifier == NULL;
            break;

        case KEFIR_AST_DECLARATOR_POINTER:
            REQUIRE_OK(kefir_ast_declarator_is_abstract(decl->pointer.declarator, result));
            break;

        case KEFIR_AST_DECLARATOR_ARRAY:
            REQUIRE_OK(kefir_ast_declarator_is_abstract(decl->array.declarator, result));
            break;

        case KEFIR_AST_DECLARATOR_FUNCTION:
            REQUIRE_OK(kefir_ast_declarator_is_abstract(decl->function.declarator, result));
            break;
    }
    return KEFIR_OK;
}
