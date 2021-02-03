#include "kefir/ast/expr_type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast/type_conv.h"

struct assign_param {
    struct kefir_ast_type_repository *repo;
    struct kefir_mem *mem;
    struct kefir_ast_node_base *base;
};

static kefir_result_t visit_non_expression(const struct kefir_ast_visitor *visitor,
                                         const struct kefir_ast_node_base *base,
                                         void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to assign type to non-expression AST node");
}

static kefir_result_t visit_constant(const struct kefir_ast_visitor *visitor,
                                   const struct kefir_ast_constant *node,
                                   void *payload) {
    UNUSED(visitor);
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            param->base->expression_type = kefir_ast_type_bool();
            break;

        case KEFIR_AST_CHAR_CONSTANT:
            param->base->expression_type = kefir_ast_type_unsigned_char();
            break;

        case KEFIR_AST_INT_CONSTANT:
            param->base->expression_type = kefir_ast_type_signed_int();
            break;

        case KEFIR_AST_UINT_CONSTANT:
            param->base->expression_type = kefir_ast_type_unsigned_int();
            break;

        case KEFIR_AST_LONG_CONSTANT:
            param->base->expression_type = kefir_ast_type_signed_long();
            break;

        case KEFIR_AST_ULONG_CONSTANT:
            param->base->expression_type = kefir_ast_type_unsigned_long();
            break;

        case KEFIR_AST_LONG_LONG_CONSTANT:
            param->base->expression_type = kefir_ast_type_signed_long_long();
            break;

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            param->base->expression_type = kefir_ast_type_unsigned_long_long();
            break;

        case KEFIR_AST_FLOAT_CONSTANT:
            param->base->expression_type = kefir_ast_type_float();
            break;

        case KEFIR_AST_DOUBLE_CONSTANT:
            param->base->expression_type = kefir_ast_type_double();
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t visit_unary_operation(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_unary_operation *node,
                                          void *payload) {
    UNUSED(visitor);
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    REQUIRE_OK(KEFIR_AST_ASSIGN_EXPRESSION_TYPE(param->mem, param->repo, node->arg));
    param->base->expression_type = node->arg->expression_type;
    return KEFIR_OK;
}

static kefir_result_t visit_binary_operation(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_binary_operation *node,
                                           void *payload) {
    UNUSED(visitor);
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    REQUIRE_OK(KEFIR_AST_ASSIGN_EXPRESSION_TYPE(param->mem, param->repo, node->arg1));
    REQUIRE_OK(KEFIR_AST_ASSIGN_EXPRESSION_TYPE(param->mem, param->repo, node->arg2));
    REQUIRE(node->arg1->expression_type->basic && node->arg2->expression_type->basic,
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Binary type derival from non-basic AST types is not supported yet"));
    param->base->expression_type = kefir_ast_type_common_arithmetic(node->arg1->expression_type, node->arg2->expression_type);
    REQUIRE(param->base->expression_type != NULL,
        KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to determine common AST arithmetic type"));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_assign_expression_type(struct kefir_mem *mem,
                                            struct kefir_ast_type_repository *repo,
                                            struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(repo != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type repository"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    struct assign_param param = {
        .repo = repo,
        .mem = mem,
        .base = base
    };
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_expression));
    visitor.constant = visit_constant;
    visitor.unary_operation = visit_unary_operation;
    visitor.binary_operation = visit_binary_operation;
    return KEFIR_AST_NODE_VISIT(&visitor, base, &param);
}