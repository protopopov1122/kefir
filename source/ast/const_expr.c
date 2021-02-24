#include "kefir/ast/const_expr.h"
#include "kefir/ast/node_base.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

struct kefir_ast_constant_expression *kefir_ast_constant_expression_empty(struct kefir_mem *mem,
                                                                      kefir_ast_constant_expression_value_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant_expression *const_expr = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant_expression));
    REQUIRE(const_expr != NULL, NULL);
    const_expr->value = value;
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
    const_expr->value = 0;
    return KEFIR_OK;
}