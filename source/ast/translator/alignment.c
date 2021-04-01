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
    kefir_ast_target_environment_opaque_type_t target_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &env->target_env, type, &target_type));
    kefir_result_t res = KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &env->target_env, target_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &env->target_env, target_type);
        return res;
    });
    *value = type_info.alignment;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &env->target_env, target_type));
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