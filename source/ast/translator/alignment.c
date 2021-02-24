#include "kefir/ast/translator/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_eval_alignment(struct kefir_mem *mem,
                                               const struct kefir_ast_alignment *alignment,
                                               const struct kefir_ast_translator_environment *env,
                                               kefir_size_t *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid alignment pointer"));
    if (alignment == NULL) {
        *value = KEFIR_AST_DEFAULT_ALIGNMENT;
        return KEFIR_OK;
    }
    switch (alignment->klass) {
        case KEFIR_AST_ALIGNMENT_DEFAULT:
            *value = 0;
            return KEFIR_OK;

        case KEFIR_AST_ALIGNMENT_AS_TYPE: {
            struct kefir_ir_target_type_info type_info;
            REQUIRE_OK(kefir_ast_translator_environment_target_stored_type_info(mem, env, alignment->type, &type_info));
            *value = type_info.alignment;
            return KEFIR_OK;
        }

        case KEFIR_AST_ALIGNMENT_AS_CONST_EXPR:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "AST alignment as constant expression is not implemented yet");
    }
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST alignment class");
}