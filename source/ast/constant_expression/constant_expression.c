#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct eval_param {
    struct kefir_mem *mem;
    const struct kefir_ast_context *context;
    struct kefir_ast_constant_expression_value *value;
};

static kefir_result_t visit_non_constant_expression(const struct kefir_ast_visitor *visitor,
                                                  const struct kefir_ast_node_base *base,
                                                  void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to evaluate non-constant AST node");
}

#define VISITOR(_id, _type) \
    kefir_result_t evaluate_##_id(const struct kefir_ast_visitor *visitor, \
                                const _type *node, \
                                void *payload) { \
        UNUSED(visitor); \
        ASSIGN_DECL_CAST(struct eval_param *, param, \
            payload); \
        return kefir_ast_evaluate_##_id##_node(param->mem, param->context, node, param->value); \
    }
VISITOR(scalar, struct kefir_ast_constant)
VISITOR(identifier, struct kefir_ast_identifier)
#undef VISITOR

kefir_result_t kefir_ast_constant_expression_evaluate(struct kefir_mem *mem,
                                                  const struct kefir_ast_context *context,
                                                  struct kefir_ast_node_base *node,
                                                  struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression value pointer"));

    REQUIRE(node->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));
    REQUIRE(node->properties.expression_props.constant_expression,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));

    struct eval_param param = {
        .mem = mem,
        .context = context,
        .value = value
    };
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_constant_expression));
    visitor.constant = evaluate_scalar;
    visitor.identifier = evaluate_identifier;
    return KEFIR_AST_NODE_VISIT(&visitor, node, &param);
}

struct kefir_ast_constant_expression *kefir_ast_constant_expression_integer(struct kefir_mem *mem,
                                                                        kefir_ast_constant_expression_int_t integer) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant_expression *const_expr = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant_expression));
    REQUIRE(const_expr != NULL, NULL);
    const_expr->value.klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER;
    const_expr->value.integer = integer;
    const_expr->expression = NULL;
    return const_expr;
}

kefir_result_t kefir_ast_constant_expression_free(struct kefir_mem *mem,
                                              struct kefir_ast_constant_expression *const_expr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(const_expr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));
    if (const_expr->expression != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, (struct kefir_ast_node_base *) const_expr->expression));
        const_expr->expression = NULL;
    }
    KEFIR_FREE(mem, const_expr);
    return KEFIR_OK;
}