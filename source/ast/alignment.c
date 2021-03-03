#include <string.h>
#include "kefir/ast/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct kefir_ast_alignment *kefir_ast_alignment_default(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_alignment *alignment = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_alignment));
    REQUIRE(alignment != NULL, NULL);
    *alignment = (const struct kefir_ast_alignment){0};
    alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
    return alignment;
}

struct kefir_ast_alignment *kefir_ast_alignment_as_type(struct kefir_mem *mem,
                                                    const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    return NULL;
}

struct kefir_ast_alignment *kefir_ast_alignment_const_expression(struct kefir_mem *mem,
                                                             struct kefir_ast_constant_expression *const_expr) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(const_expr != NULL, NULL);
    struct kefir_ast_alignment *alignment = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_alignment));
    REQUIRE(alignment != NULL, NULL);
    *alignment = (const struct kefir_ast_alignment){0};
    alignment->klass = KEFIR_AST_ALIGNMENT_AS_CONST_EXPR;
    alignment->value = const_expr->value;
    alignment->const_expr = const_expr;
    return alignment;
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
            alignment->const_expr = NULL;
            break;
    }
    KEFIR_FREE(mem, alignment);
    return KEFIR_OK;
}