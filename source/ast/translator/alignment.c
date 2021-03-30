#include "kefir/ast/translator/alignment.h"
#include "kefir/ast/target_environment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_type_alignment(struct kefir_mem *mem,
                                               const struct kefir_ast_type *type,
                                               const struct kefir_ast_translator_environment *env,
                                               kefir_size_t *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid alignment pointer"));
    struct kefir_ast_target_type_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_TYPE_INFO(mem, &env->target_env, type, &type_info));
    *value = type_info.alignment;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_eval_alignment(struct kefir_mem *mem,
                                               struct kefir_ast_alignment *alignment,
                                               const struct kefir_ast_translator_environment *env) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    switch (alignment->klass) {
        case KEFIR_AST_ALIGNMENT_DEFAULT:
            alignment->value = KEFIR_AST_DEFAULT_ALIGNMENT;
            break;

        case KEFIR_AST_ALIGNMENT_AS_TYPE:
            REQUIRE_OK(kefir_ast_translator_type_alignment(mem, alignment->type, env, &alignment->value));
            break;

        case KEFIR_AST_ALIGNMENT_AS_CONST_EXPR:
            // TODO: Add constant expression evaluator
            break;
    }
    return KEFIR_OK;
}