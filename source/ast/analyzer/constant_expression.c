#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_constant_expression(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     struct kefir_ast_constant_expression *const_expr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(const_expr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));

    if (const_expr->expression != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, const_expr->expression));
    }
    return KEFIR_OK;
}
