#include "kefir/ast/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_alignment_default(struct kefir_ast_alignment *alignment) {
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
    alignment->type = NULL;
    alignment->const_expr = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_alignment_as_type(struct kefir_ast_alignment *alignment,
                                       const struct kefir_ast_type *type) {
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    alignment->klass = KEFIR_AST_ALIGNMENT_AS_TYPE;
    alignment->type = type;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_alignment_const_expression(struct kefir_ast_alignment *alignment,
                                                kefir_ast_constant_expression_t const_expr) {
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    alignment->klass = KEFIR_AST_ALIGNMENT_AS_TYPE;
    alignment->const_expr = const_expr;
    return KEFIR_OK;
}