#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t ast_constant_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_constant *, node,
        base->self);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_CONSTANT_CLASS = {
    .type = KEFIR_AST_CONSTANT,
    .free = ast_constant_free
};

struct kefir_ast_node_base *kefir_ast_new_constant_i64(struct kefir_mem *mem, kefir_int64_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->type = KEFIR_AST_INT64_CONSTANT;
    constant->value.i64 = value;
    return (struct kefir_ast_node_base *) constant;
}

struct kefir_ast_node_base *kefir_ast_new_constant_f32(struct kefir_mem *mem, kefir_float32_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->type = KEFIR_AST_FLOAT32_CONSTANT;
    constant->value.f32 = value;
    return (struct kefir_ast_node_base *) constant;
}

struct kefir_ast_node_base *kefir_ast_new_constant_f64(struct kefir_mem *mem, kefir_float64_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->type = KEFIR_AST_FLOAT64_CONSTANT;
    constant->value.f64 = value;
    return (struct kefir_ast_node_base *) constant;
}

kefir_result_t ast_unary_operation_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_unary_operation *, node,
        base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->arg));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_UNARY_OPERATION_CLASS = {
    .type = KEFIR_AST_UNARY_OPERATION,
    .free = ast_unary_operation_free
};

struct kefir_ast_node_base *kefir_ast_new_unary_operation(struct kefir_mem *mem,
                                                      kefir_ast_unary_operation_type_t type,
                                                      struct kefir_ast_node_base *arg) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(arg != NULL, NULL);
    struct kefir_ast_unary_operation *oper = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_unary_operation));
    REQUIRE(oper != NULL, NULL);
    oper->base.klass = &AST_UNARY_OPERATION_CLASS;
    oper->type = type;
    oper->arg = arg;
    return (struct kefir_ast_node_base *) oper;
}

kefir_result_t ast_binary_operation_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_unary_operation *, node,
        base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->arg));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_BINARY_OPERATION_CLASS = {
    .type = KEFIR_AST_BINARY_OPERATION,
    .free = ast_binary_operation_free
};

struct kefir_ast_node_base *kefir_ast_new_binary_operation(struct kefir_mem *mem,
                                                       kefir_ast_binary_operation_type_t type,
                                                       struct kefir_ast_node_base *arg1,
                                                       struct kefir_ast_node_base *arg2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(arg1 != NULL, NULL);
    REQUIRE(arg2 != NULL, NULL);
    struct kefir_ast_binary_operation *oper = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_binary_operation));
    REQUIRE(oper != NULL, NULL);
    oper->base.klass = &AST_BINARY_OPERATION_CLASS;
    oper->type = type;
    oper->arg1 = arg1;
    oper->arg2 = arg2;
    return (struct kefir_ast_node_base *) oper;
}

kefir_result_t kefir_ast_visitor_init(struct kefir_ast_visitor *visitor,
                                  kefir_result_t (*generic)(const struct kefir_ast_visitor *, const struct kefir_ast_node_base *, void *)) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST visitor"));
    *visitor = (const struct kefir_ast_visitor){0};
    visitor->generic_handler = generic;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_visitor_visit(const struct kefir_ast_visitor *visitor,
                                   const struct kefir_ast_node_base *base,
                                   void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST visitor"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));
#define DISPATCH(func, type) \
    do { \
        if (visitor->func != NULL) { \
            return visitor->func(visitor, ((type *) base->self), payload); \
        } else if (visitor->generic_handler != NULL) { \
            return visitor->generic_handler(visitor, base, payload); \
        } else { \
            return KEFIR_OK; \
        } \
    } while (0)
    switch (base->klass->type) {
        case KEFIR_AST_CONSTANT:
            DISPATCH(constant, struct kefir_ast_constant);
            break;

        case KEFIR_AST_UNARY_OPERATION:
            DISPATCH(unary_operation, struct kefir_ast_unary_operation);
            break;

        case KEFIR_AST_BINARY_OPERATION:
            DISPATCH(binary_operation, struct kefir_ast_binary_operation);
            break;
    }
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST node type");
}