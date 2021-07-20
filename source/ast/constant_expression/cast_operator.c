#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_evaluate_cast_operator_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_cast_operator *node,
                                                     struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant node"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Expected constant expression AST node"));

    struct kefir_ast_constant_expression_value arg_value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->expr, &arg_value));

    const struct kefir_ast_type *unqualified = kefir_ast_unqualified_type(node->type_name->base.properties.type);
    if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(unqualified)) {
        value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
        switch (arg_value.klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                value->integer = arg_value.integer;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                value->integer = (kefir_ast_constant_expression_int_t) arg_value.floating_point;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
                value->pointer = arg_value.pointer;
                value->pointer.pointer_node = KEFIR_AST_NODE_BASE(node);
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
        }
    } else if (KEFIR_AST_TYPE_IS_FLOATING_POINT(unqualified)) {
        value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT;
        switch (arg_value.klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                value->floating_point = (kefir_ast_constant_expression_float_t) arg_value.integer;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                value->floating_point = arg_value.floating_point;
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                       "Address to floating point cast is not a constant expression");

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
        }
    } else if (unqualified->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE(arg_value.klass != KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot cast floating point to address"));

        value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
        switch (arg_value.klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                value->pointer.type = KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER;
                value->pointer.base.integral = arg_value.integer;
                value->pointer.offset = 0;
                value->pointer.pointer_node = KEFIR_AST_NODE_BASE(node);
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to cast floating point to address");

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                value->pointer = arg_value.pointer;
                value->pointer.pointer_node = KEFIR_AST_NODE_BASE(node);
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Non-evaluated constant expression");
        }
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Expected constant expression");
    }
    return KEFIR_OK;
}
