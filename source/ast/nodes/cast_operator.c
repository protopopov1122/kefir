#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_cast_operator_visit, kefir_ast_cast_operator, cast_operator)

struct kefir_ast_node_base *ast_cast_operator_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_cast_operator_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_cast_operator *, node,
        base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->expr));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_CAST_OPERATOR_CLASS = {
    .type = KEFIR_AST_CAST_OPERATOR,
    .visit = ast_cast_operator_visit,
    .clone = ast_cast_operator_clone,
    .free = ast_cast_operator_free
};

struct kefir_ast_node_base *ast_cast_operator_clone(struct kefir_mem *mem,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_cast_operator *, node,
        base->self);
    struct kefir_ast_cast_operator *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_cast_operator));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_CAST_OPERATOR_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->type = node->type;
    clone->expr = KEFIR_AST_NODE_CLONE(mem, node->expr);
    REQUIRE_ELSE(clone->expr != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_cast_operator *kefir_ast_new_cast_operator(struct kefir_mem *mem,
                                                        const struct kefir_ast_type *type,
                                                        struct kefir_ast_node_base *expr) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    REQUIRE(expr != NULL, NULL);
    struct kefir_ast_cast_operator *cast = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_cast_operator));
    REQUIRE(cast != NULL, NULL);
    cast->base.klass = &AST_CAST_OPERATOR_CLASS;
    cast->base.self = cast;
    kefir_result_t res = kefir_ast_node_properties_init(&cast->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, cast);
        return NULL;
    });
    cast->type = type;
    cast->expr = expr;
    return cast;
}
