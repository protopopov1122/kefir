#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_type_name_visit, kefir_ast_type_name, type_name)

struct kefir_ast_node_base *ast_type_name_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_type_name_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_name *, node,
        base->self);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &node->type_decl.specifiers));
    REQUIRE_OK(kefir_ast_declarator_free(mem, node->type_decl.declarator));
    node->type_decl.declarator = NULL;
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_TYPE_NAME_CLASS = {
    .type = KEFIR_AST_TYPE_NAME,
    .visit = ast_type_name_visit,
    .clone = ast_type_name_clone,
    .free = ast_type_name_free
};

struct kefir_ast_node_base *ast_type_name_clone(struct kefir_mem *mem,
                                              struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_type_name *, node,
        base->self);
    struct kefir_ast_type_name *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_name));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_TYPE_NAME_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    
    clone->type_decl.declarator = kefir_ast_declarator_clone(mem, node->type_decl.declarator);
    REQUIRE_ELSE(clone->type_decl.declarator != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_init(&clone->type_decl.specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, clone->type_decl.declarator);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_clone(mem, &clone->type_decl.specifiers, &node->type_decl.specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &clone->type_decl.specifiers);
        kefir_ast_declarator_free(mem, clone->type_decl.declarator);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_type_name *kefir_ast_new_type_name(struct kefir_mem *mem,
                                                struct kefir_ast_declarator *decl) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(decl != NULL, NULL);

    kefir_bool_t abstract = false;
    REQUIRE(kefir_ast_declarator_is_abstract(decl, &abstract) == KEFIR_OK && abstract, NULL);

    struct kefir_ast_type_name *type_name = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_name));
    REQUIRE(type_name != NULL, NULL);
    type_name->base.klass = &AST_TYPE_NAME_CLASS;
    type_name->base.self = type_name;
    kefir_result_t res = kefir_ast_node_properties_init(&type_name->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type_name);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_init(&type_name->type_decl.specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type_name);
        return NULL;
    });
    type_name->type_decl.declarator = decl;
    return type_name;
}
