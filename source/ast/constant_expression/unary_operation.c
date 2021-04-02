#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_evaluate_unary_operation_node(struct kefir_mem *mem,
                                                   const struct kefir_ast_context *context,
                                                   const struct kefir_ast_unary_operation *node,
                                                   struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST unary operation node"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION &&
        node->base.properties.expression_props.constant_expression,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));

    struct kefir_ast_constant_expression_value arg_value;
    switch (node->type) {
            case KEFIR_AST_OPERATION_PLUS:
                REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                    value->integer = arg_value.integer;
                } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                    value->floating_point = arg_value.floating_point;
                } else {
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integeral or floating-point constant expression");
                }
                break;
                
            case KEFIR_AST_OPERATION_NEGATE:
                REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                    value->integer = -arg_value.integer;
                } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
                    value->floating_point = -arg_value.floating_point;
                } else {
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integeral or floating-point constant expression");
                }
                break;
                
            case KEFIR_AST_OPERATION_INVERT:
                REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                    value->integer = ~arg_value.integer;
                } else {
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integeral constant expression");
                }
                break;
                
            case KEFIR_AST_OPERATION_LOGICAL_NEGATE:
                REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->arg, &arg_value));
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER) {
                    value->integer = !arg_value.integer;
                } else if (arg_value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT) {
                    value->integer = !arg_value.floating_point;
                } else {
                    switch (arg_value.pointer.type) {
                        case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER:
                        case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL:
                            value->integer = false;
                            break;

                        case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER:
                            value->integer = !(kefir_bool_t) (arg_value.pointer.base.integral + arg_value.pointer.offset);
                            break;
                    }
                }
                break;
                
            case KEFIR_AST_OPERATION_POSTFIX_INCREMENT:
            case KEFIR_AST_OPERATION_POSTFIX_DECREMENT:
            case KEFIR_AST_OPERATION_PREFIX_INCREMENT:
            case KEFIR_AST_OPERATION_PREFIX_DECREMENT:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                    "Constant expressions shall not contain increment/decrement operators");
                
            case KEFIR_AST_OPERATION_ADDRESS:
                REQUIRE_OK(kefir_ast_constant_expression_value_evaluate_lvalue_reference(mem, context,
                    node->arg, &value->pointer));
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
                break;
                
            case KEFIR_AST_OPERATION_INDIRECTION:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                    "Constant expression cannot contain indirection operator");
                
            case KEFIR_AST_OPERATION_SIZEOF: {
                kefir_ast_target_environment_opaque_type_t opaque_type;
                struct kefir_ast_target_environment_object_info type_info;
                REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env,
                    node->arg->properties.type, &opaque_type));
                kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env,
                    opaque_type, NULL, &type_info);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
                    return res;
                });
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = type_info.size;
                REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type));
            } break;

            case KEFIR_AST_OPERATION_ALIGNOF: {
                kefir_ast_target_environment_opaque_type_t opaque_type;
                struct kefir_ast_target_environment_object_info type_info;
                REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env,
                    node->arg->properties.type, &opaque_type));
                kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env,
                    opaque_type, NULL, &type_info);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type);
                    return res;
                });
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
                value->integer = type_info.alignment;
                REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, opaque_type));
            } break;
    }
    return KEFIR_OK;
}