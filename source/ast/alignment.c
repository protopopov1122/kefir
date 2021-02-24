#include <string.h>
#include "kefir/ast/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_alignment_default(struct kefir_ast_alignment *alignment) {
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    *alignment = (const struct kefir_ast_alignment){0};
    alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
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
                                                struct kefir_ast_constant_expression *const_expr) {
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    REQUIRE(const_expr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));
    alignment->klass = KEFIR_AST_ALIGNMENT_AS_CONST_EXPR;
    alignment->const_expr = const_expr;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_alignment_free(struct kefir_mem *mem,
                                    struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    switch (alignment->klass) {
        case KEFIR_AST_ALIGNMENT_DEFAULT:
            break;

        case KEFIR_AST_ALIGNMENT_AS_TYPE:
            alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
            alignment->type = NULL;
            break;
        
        case KEFIR_AST_ALIGNMENT_AS_CONST_EXPR:
            alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
            REQUIRE_OK(kefir_ast_constant_expression_free(mem, alignment->const_expr));
            KEFIR_FREE(mem, alignment->const_expr);
            alignment->const_expr = NULL;
            break;
    }
    return KEFIR_OK;
}