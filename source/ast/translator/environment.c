#include "kefir/ast/translator/environment.h"
#include "kefir/ast/translator/translator.h"
#include "kefir/ast/alignment.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t target_stored_type_info(struct kefir_mem *mem,
                                            const struct kefir_ast_translator_environment *env,
                                            const struct kefir_ast_type *type,
                                            struct kefir_ir_target_type_info *type_info) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(type_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR target type info"));

    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    kefir_result_t res = kefir_irbuilder_type_init(mem, &builder, &ir_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    res = kefir_ast_translate_stored_object_type(mem, type, KEFIR_AST_DEFAULT_ALIGNMENT, env, &builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&builder);
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    res = KEFIR_IRBUILDER_TYPE_FREE(&builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    res = KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(env->target_platform, mem, &ir_type, type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &ir_type);
        return res;
    });
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}

static kefir_result_t target_env_type_info(struct kefir_mem *mem,
                                         const struct kefir_ast_target_environment *env,
                                         const struct kefir_ast_type *type,
                                         struct kefir_ast_target_type_info *info) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment type info"));
    
    struct kefir_ir_target_type_info ir_type_info;
    REQUIRE_OK(target_stored_type_info(mem, env->payload, type, &ir_type_info));
    info->size = ir_type_info.size;
    info->alignment = ir_type_info.alignment;
    info->aligned = ir_type_info.aligned;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_environment_init(struct kefir_ast_translator_environment *env,
                                                 struct kefir_ir_target_platform *target_platform) {    
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(target_platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid IR target platform"));
    env->target_platform = target_platform;
    env->target_env.type_info = target_env_type_info;
    env->target_env.payload = env;
    return KEFIR_OK;
}
