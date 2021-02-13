#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define NODE_VISIT_IMPL(identifier, type, handler) \
    kefir_result_t identifier(const struct kefir_ast_node_base *base, \
                            const struct kefir_ast_visitor *visitor, \
                            void *payload) { \
        REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base")); \
        REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node visitor")); \
        ASSIGN_DECL_CAST(const struct type *, node, \
            base->self); \
        if (visitor->handler != NULL) { \
            return visitor->handler(visitor, node, payload); \
        } else if (visitor->generic_handler != NULL) { \
            return visitor->generic_handler(visitor, base, payload); \
        } else { \
            return KEFIR_OK; \
        } \
    }

NODE_VISIT_IMPL(ast_constant_visit, kefir_ast_constant, constant)

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
    .visit = ast_constant_visit,
    .free = ast_constant_free
};

struct kefir_ast_constant *kefir_ast_new_constant_bool(struct kefir_mem *mem, kefir_bool_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_BOOL_CONSTANT;
    constant->value.boolean = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_char(struct kefir_mem *mem, kefir_char_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_CHAR_CONSTANT;
    constant->value.character = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_int(struct kefir_mem *mem, kefir_int_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_INT_CONSTANT;
    constant->value.integer = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_uint(struct kefir_mem *mem, kefir_uint_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_UINT_CONSTANT;
    constant->value.uinteger = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_long(struct kefir_mem *mem, kefir_long_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_LONG_CONSTANT;
    constant->value.long_integer = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_ulong(struct kefir_mem *mem, kefir_ulong_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_ULONG_CONSTANT;
    constant->value.ulong_integer = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_long_long(struct kefir_mem *mem, kefir_long_long_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_LONG_LONG_CONSTANT;
    constant->value.long_long = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_ulong_long(struct kefir_mem *mem, kefir_ulong_long_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_ULONG_LONG_CONSTANT;
    constant->value.ulong_long = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_float(struct kefir_mem *mem, kefir_float32_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_FLOAT_CONSTANT;
    constant->value.float32 = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_double(struct kefir_mem *mem, kefir_float64_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    constant->base.expression_type = NULL;
    constant->type = KEFIR_AST_DOUBLE_CONSTANT;
    constant->value.float64 = value;
    return constant;
}

NODE_VISIT_IMPL(ast_identifier_visit, kefir_ast_identifier, identifier)

kefir_result_t ast_identifier_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_identifier *, node,
        base->self);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_IDENTIFIER_CLASS = {
    .type = KEFIR_AST_IDENTIFIER,
    .visit = ast_identifier_visit,
    .free = ast_identifier_free
};

struct kefir_ast_identifier *kefir_ast_new_identifier(struct kefir_mem *mem,
                                                  struct kefir_symbol_table *symbols,
                                                  const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(symbols != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);
    const char *id_copy = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
    REQUIRE(id_copy != NULL, NULL);
    struct kefir_ast_identifier *id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_identifier));
    REQUIRE(id != NULL, NULL);
    id->base.klass = &AST_IDENTIFIER_CLASS;
    id->base.self = id;
    id->base.expression_type = NULL;
    id->identifier = id_copy;
    return id;
}

NODE_VISIT_IMPL(ast_unary_operation_visit, kefir_ast_unary_operation, unary_operation)

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
    .visit = ast_unary_operation_visit,
    .free = ast_unary_operation_free
};

struct kefir_ast_unary_operation *kefir_ast_new_unary_operation(struct kefir_mem *mem,
                                                            kefir_ast_unary_operation_type_t type,
                                                            struct kefir_ast_node_base *arg) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(arg != NULL, NULL);
    struct kefir_ast_unary_operation *oper = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_unary_operation));
    REQUIRE(oper != NULL, NULL);
    oper->base.klass = &AST_UNARY_OPERATION_CLASS;
    oper->base.self = oper;
    oper->base.expression_type = NULL;
    oper->type = type;
    oper->arg = arg;
    return oper;
}

NODE_VISIT_IMPL(ast_binary_operation_visit, kefir_ast_binary_operation, binary_operation)

kefir_result_t ast_binary_operation_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_binary_operation *, node,
        base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->arg1));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->arg2));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_BINARY_OPERATION_CLASS = {
    .type = KEFIR_AST_BINARY_OPERATION,
    .visit = ast_binary_operation_visit,
    .free = ast_binary_operation_free
};

struct kefir_ast_binary_operation *kefir_ast_new_binary_operation(struct kefir_mem *mem,
                                                              kefir_ast_binary_operation_type_t type,
                                                              struct kefir_ast_node_base *arg1,
                                                              struct kefir_ast_node_base *arg2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(arg1 != NULL, NULL);
    REQUIRE(arg2 != NULL, NULL);
    struct kefir_ast_binary_operation *oper = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_binary_operation));
    REQUIRE(oper != NULL, NULL);
    oper->base.klass = &AST_BINARY_OPERATION_CLASS;
    oper->base.self = oper;
    oper->base.expression_type = NULL;
    oper->type = type;
    oper->arg1 = arg1;
    oper->arg2 = arg2;
    return oper;
}

kefir_result_t kefir_ast_visitor_init(struct kefir_ast_visitor *visitor,
                                  kefir_result_t (*generic)(const struct kefir_ast_visitor *, const struct kefir_ast_node_base *, void *)) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST visitor"));
    *visitor = (const struct kefir_ast_visitor){0};
    visitor->generic_handler = generic;
    return KEFIR_OK;
}