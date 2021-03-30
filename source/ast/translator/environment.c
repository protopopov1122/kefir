#include <string.h>
#include "kefir/ast/translator/environment.h"
#include "kefir/ast/translator/translator.h"
#include "kefir/ast/alignment.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t target_type_info(struct kefir_mem *mem,
                                     const struct kefir_ast_target_environment *target_env,
                                     const struct kefir_ast_type *type,
                                     struct kefir_ast_target_type_info *type_info) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(target_env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST target environment"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(type_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target type info"));
    ASSIGN_DECL_CAST(const struct kefir_ast_translator_environment *, env,
        target_env->payload);

    struct kefir_ast_translator_environment_type *env_type =
         KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_environment_type));
    REQUIRE(env_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator environment type"));

    struct kefir_irbuilder_type builder;
    kefir_result_t res = kefir_ir_type_alloc(mem, 0, &env_type->type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, env_type);
        return res;
    });
    res = kefir_irbuilder_type_init(mem, &builder, &env_type->type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });
    res = kefir_ast_translate_object_type(mem, type, KEFIR_AST_DEFAULT_ALIGNMENT, env, &builder, &env_type->layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&builder);
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });
    res = KEFIR_IRBUILDER_TYPE_FREE(&builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });

    struct kefir_ir_target_type_info ir_type_info;
    res = KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(env->target_platform, mem, &env_type->type, &ir_type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });

    env_type->ast_type = type;
    type_info->type = env_type;
    type_info->size = ir_type_info.size;
    type_info->alignment = ir_type_info.alignment;
    type_info->aligned = ir_type_info.aligned;
    return KEFIR_OK;
}

static kefir_result_t free_type_info(struct kefir_mem *mem,
                                   const struct kefir_ast_target_environment *env,
                                   struct kefir_ast_target_type_info *type_info) {
    UNUSED(env);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target type info"));
    
    ASSIGN_DECL_CAST(struct kefir_ast_translator_environment_type *, env_type,
        type_info->type);
    REQUIRE_OK(kefir_ast_type_layout_free(mem, env_type->layout));
    env_type->layout = NULL;
    REQUIRE_OK(kefir_ir_type_free(mem, &env_type->type));
    *env_type = (struct kefir_ast_translator_environment_type){0};
    KEFIR_FREE(mem, env_type);
    return KEFIR_OK;
}

static kefir_result_t offsetof_member(struct kefir_mem *mem,
                                    const struct kefir_ast_target_environment *env,
                                    kefir_ast_target_environment_type_t env_type,
                                    const char *member,
                                    kefir_size_t *offset_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment"));
    REQUIRE(env_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment type"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_environment_type *, type,
        env_type);
    REQUIRE(type->ast_type->tag == KEFIR_AST_TYPE_STRUCTURE || type->ast_type->tag == KEFIR_AST_TYPE_UNION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected structure/union type"));
    REQUIRE(member != NULL && strlen(member) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid structure member identifier"));
    REQUIRE(offset_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid offset pointer"));

    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Target environment struct/union offsetof is not implemented yet");
}

static kefir_result_t offsetof_element(struct kefir_mem *mem,
                                     const struct kefir_ast_target_environment *env,
                                     kefir_ast_target_environment_type_t env_type,
                                     kefir_int64_t index,
                                     kefir_size_t *offset_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment"));
    REQUIRE(env_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment type"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_environment_type *, type,
        env_type);
    REQUIRE(type->ast_type->tag == KEFIR_AST_TYPE_ARRAY || type->ast_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected array/pointer type"));
    REQUIRE(offset_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid offset pointer"));
    UNUSED(index);

    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Target environment array/pointer offsetof is not implemented yet");
}

kefir_result_t kefir_ast_translator_environment_init(struct kefir_ast_translator_environment *env,
                                                 struct kefir_ir_target_platform *target_platform) {    
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(target_platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid IR target platform"));
    env->target_platform = target_platform;
    env->target_env.type_info = target_type_info;
    env->target_env.free_type_info = free_type_info;
    env->target_env.offsetof_member = offsetof_member;
    env->target_env.offsetof_element = offsetof_element;
    env->target_env.payload = env;
    return KEFIR_OK;
}
