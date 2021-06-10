#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_unary_operation_visit, kefir_ast_unary_operation, unary_operation)

struct kefir_ast_node_base *ast_unary_operation_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_unary_operation_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_unary_operation *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->arg));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_UNARY_OPERATION_CLASS = {.type = KEFIR_AST_UNARY_OPERATION,
                                                               .visit = ast_unary_operation_visit,
                                                               .clone = ast_unary_operation_clone,
                                                               .free = ast_unary_operation_free};

struct kefir_ast_node_base *ast_unary_operation_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_unary_operation *, node, base->self);
    struct kefir_ast_unary_operation *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_unary_operation));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_UNARY_OPERATION_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->type = node->type;
    clone->arg = KEFIR_AST_NODE_CLONE(mem, node->arg);
    REQUIRE_ELSE(clone->arg != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_unary_operation *kefir_ast_new_unary_operation(struct kefir_mem *mem,
                                                                kefir_ast_unary_operation_type_t type,
                                                                struct kefir_ast_node_base *arg) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(arg != NULL, NULL);
    struct kefir_ast_unary_operation *oper = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_unary_operation));
    REQUIRE(oper != NULL, NULL);
    oper->base.klass = &AST_UNARY_OPERATION_CLASS;
    oper->base.self = oper;
    kefir_result_t res = kefir_ast_node_properties_init(&oper->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, oper);
        return NULL;
    });
    oper->type = type;
    oper->arg = arg;
    return oper;
}
