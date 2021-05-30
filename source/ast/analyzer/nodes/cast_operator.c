#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_cast_operator_node(struct kefir_mem *mem,
                                                const struct kefir_ast_context *context,
                                                const struct kefir_ast_cast_operator *cast,
                                                struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(cast != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST cast operator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(cast->type_name)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, cast->expr));
    REQUIRE(cast->expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cast operator operand shall be an expression"));

    const struct kefir_ast_type *expr_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, cast->expr->properties.type);
    const struct kefir_ast_type *cast_type =
        kefir_ast_unqualified_type(cast->type_name->base.properties.type);
    REQUIRE((KEFIR_AST_TYPE_IS_SCALAR_TYPE(expr_type) && KEFIR_AST_TYPE_IS_SCALAR_TYPE(cast_type)) ||
        cast->type_name->base.properties.type->tag == KEFIR_AST_TYPE_VOID,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cast should involve scalar types unless type name is void"));
    if (KEFIR_AST_TYPE_IS_FLOATING_POINT(cast_type)) {
        REQUIRE(expr_type->tag != KEFIR_AST_TYPE_SCALAR_POINTER,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Pointer cannot be cast to floating-point value"));
    }
    if (KEFIR_AST_TYPE_IS_FLOATING_POINT(expr_type)) {
        REQUIRE(cast_type->tag != KEFIR_AST_TYPE_SCALAR_POINTER,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Floating-point value cannot be cast to pointer"));
    }

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = cast->type_name->base.properties.type;
    base->properties.expression_props.constant_expression = cast->expr->properties.expression_props.constant_expression;
    return KEFIR_OK;
}

