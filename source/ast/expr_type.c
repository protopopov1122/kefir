#include "kefir/ast/expr_type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast/type_conv.h"

struct assign_param {
    struct kefir_mem *mem;
    struct kefir_ast_node_base *base;
    const struct kefir_ast_context *context;
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

static kefir_result_t visit_identifier(const struct kefir_ast_visitor *visitor,
                                     const struct kefir_ast_identifier *node,
                                     void *payload) {
    UNUSED(visitor);
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(kefir_ast_context_resolve_object_identifier(
        param->context, node->identifier, &scoped_id));
    switch (scoped_id->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            param->base->expression_type = scoped_id->object.type;
            return KEFIR_OK;
        
        default:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Non-object identifiers are not supported yet");
    }
}

static kefir_result_t visit_unary_operation(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_unary_operation *node,
                                          void *payload) {
    UNUSED(visitor);
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    REQUIRE_OK(KEFIR_AST_ASSIGN_EXPRESSION_TYPE(param->mem, param->context, node->arg));
    const struct kefir_ast_type *type1 = kefir_ast_unqualified_type(node->arg->expression_type);
    switch (node->type) {
        case KEFIR_AST_OPERATION_PLUS:
        case KEFIR_AST_OPERATION_NEGATE:
            REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected arithmetic argument of unary +|-"));
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1)) {
                param->base->expression_type = kefir_ast_type_int_promotion(param->context->global->type_traits, type1);
            } else {
                param->base->expression_type = type1;
            }
            break;
        
        case KEFIR_AST_OPERATION_INVERT:
            REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integral argument of bitwise inversion"));
            param->base->expression_type = kefir_ast_type_int_promotion(param->context->global->type_traits, type1);
            break;

        case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
            REQUIRE(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type1),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected type argument of logical negation"));
            param->base->expression_type = kefir_ast_type_signed_int();
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected unary AST node type");
    }
    param->base->expression_type = type1;
    return KEFIR_OK;
}

static kefir_result_t visit_binary_operation(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_binary_operation *node,
                                           void *payload) {
    UNUSED(visitor);
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    REQUIRE_OK(KEFIR_AST_ASSIGN_EXPRESSION_TYPE(param->mem, param->context, node->arg1));
    REQUIRE_OK(KEFIR_AST_ASSIGN_EXPRESSION_TYPE(param->mem, param->context, node->arg2));
    const struct kefir_ast_type *type1 = kefir_ast_unqualified_type(node->arg1->expression_type);
    const struct kefir_ast_type *type2 = kefir_ast_unqualified_type(node->arg2->expression_type);
    if (node->type == KEFIR_AST_OPERATION_SHIFT_LEFT ||
        node->type == KEFIR_AST_OPERATION_SHIFT_RIGHT) {
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type1) &&
            KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type2),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bitwise shift operator expects integer arguments"));
        param->base->expression_type = kefir_ast_type_int_promotion(param->context->global->type_traits, type1);
        REQUIRE(param->base->expression_type != NULL,
            KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to determine common AST arithmetic type"));
    } else if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1) &&
        KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2)) {
        param->base->expression_type = kefir_ast_type_common_arithmetic(param->context->global->type_traits, type1, type2);
        REQUIRE(param->base->expression_type != NULL,
            KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to determine common AST arithmetic type"));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Binary type derival from non-arithmetic AST types is not supported yet");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_assign_expression_type(struct kefir_mem *mem,
                                            const struct kefir_ast_context *context,
                                            struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translaction_context"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    struct assign_param param = {
        .mem = mem,
        .context = context,
        .base = base
    };
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_expression));
    visitor.constant = visit_constant;
    visitor.identifier = visit_identifier;
    visitor.unary_operation = visit_unary_operation;
    visitor.binary_operation = visit_binary_operation;
    return KEFIR_AST_NODE_VISIT(&visitor, base, &param);
}