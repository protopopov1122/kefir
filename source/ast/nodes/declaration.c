#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_declaration_visit, kefir_ast_declaration, declaration)

struct kefir_ast_node_base *ast_declaration_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_declaration_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, node,
        base->self);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &node->specifiers));
    if (node->declarator != NULL) {
        REQUIRE_OK(kefir_ast_declarator_free(mem, node->declarator));
        node->declarator = NULL;
    }
    if (node->initializer != NULL) {
        REQUIRE_OK(kefir_ast_initializer_free(mem, node->initializer));
        node->initializer = NULL;
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_DECLARATION_CLASS = {
    .type = KEFIR_AST_DECLARATION,
    .visit = ast_declaration_visit,
    .clone = ast_declaration_clone,
    .free = ast_declaration_free
};

struct kefir_ast_node_base *ast_declaration_clone(struct kefir_mem *mem,
                                              struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, node,
        base->self);
    struct kefir_ast_declaration *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declaration));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_DECLARATION_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    
    if (node->declarator != NULL) {
        clone->declarator = kefir_ast_declarator_clone(mem, node->declarator);
        REQUIRE_ELSE(clone->declarator != NULL, {
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->declarator = NULL;
    }

    res = kefir_ast_declarator_specifier_list_init(&clone->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, clone->declarator);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_clone(mem, &clone->specifiers, &node->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, clone->declarator);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    if (node->initializer != NULL) {
        clone->initializer = kefir_ast_initializer_clone(mem, node->initializer);
        REQUIRE_ELSE(clone->initializer != NULL, {
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            kefir_ast_declarator_free(mem, clone->declarator);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->initializer = NULL;
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_declaration *kefir_ast_new_declaration(struct kefir_mem *mem,
                                                        struct kefir_ast_declarator *decl,
                                                        struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_declaration *declaration = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declaration));
    REQUIRE(declaration != NULL, NULL);
    declaration->base.klass = &AST_DECLARATION_CLASS;
    declaration->base.self = declaration;
    kefir_result_t res = kefir_ast_node_properties_init(&declaration->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_init(&declaration->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration);
        return NULL;
    });
    declaration->declarator = decl;
    declaration->initializer = initializer;
    return declaration;
}
