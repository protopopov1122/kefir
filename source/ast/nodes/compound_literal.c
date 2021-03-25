#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_compound_literal_visit, kefir_ast_compound_literal, compound_literal)

struct kefir_ast_node_base *ast_compound_literal_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_compound_literal_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_compound_literal *, node,
        base->self);
    REQUIRE_OK(kefir_ast_initializer_free(mem, node->initializer));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_COMPOUND_LITERAL_CLASS = {
    .type = KEFIR_AST_COMPOUND_LITERAL,
    .visit = ast_compound_literal_visit,
    .clone = ast_compound_literal_clone,
    .free = ast_compound_literal_free
};

struct kefir_ast_node_base *ast_compound_literal_clone(struct kefir_mem *mem,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_compound_literal *, node,
        base->self);
    struct kefir_ast_compound_literal *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_compound_literal));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_COMPOUND_LITERAL_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->type = node->type;
    clone->initializer = kefir_ast_initializer_clone(mem, node->initializer);
    REQUIRE_ELSE(clone->initializer != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_compound_literal *kefir_ast_new_compound_literal(struct kefir_mem *mem,
                                                              const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    struct kefir_ast_compound_literal *literal = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_compound_literal));
    REQUIRE(literal != NULL, NULL);
    literal->base.klass = &AST_COMPOUND_LITERAL_CLASS;
    literal->base.self = literal;
    kefir_result_t res = kefir_ast_node_properties_init(&literal->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, literal);
        return NULL;
    });
    literal->type = type;
    literal->initializer = kefir_ast_new_list_initializer(mem);
    REQUIRE_ELSE(literal->initializer != NULL, {
        KEFIR_FREE(mem, literal);
        return NULL;
    });
    return literal;
}