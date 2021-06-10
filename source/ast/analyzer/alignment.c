#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_alignment(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                           struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));

    switch (alignment->klass) {
        case KEFIR_AST_ALIGNMENT_DEFAULT:
        case KEFIR_AST_ALIGNMENT_AS_TYPE:
            break;

        case KEFIR_AST_ALIGNMENT_AS_CONST_EXPR:
            REQUIRE_OK(kefir_ast_analyze_constant_expression(mem, context, alignment->const_expr));
            break;
    }
    return KEFIR_OK;
}
