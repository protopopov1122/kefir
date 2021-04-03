#include <string.h>
#include "kefir/ast-translator/environment.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/ast/alignment.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t target_env_get_type(struct kefir_mem *mem,
                                        const struct kefir_ast_target_environment *target_env,
                                        const struct kefir_ast_type *type,
                                        kefir_ast_target_environment_opaque_type_t *opaque_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(target_env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST target environment"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(opaque_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment type pointer"));
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

    res = kefir_ast_translator_evaluate_type_layout(mem, env, env_type->layout, &env_type->type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });

    res = KEFIR_IR_TARGET_PLATFORM_GET_TYPE(mem, env->target_platform, &env_type->type, &env_type->target_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });

    struct kefir_ir_target_type_info ir_type_info;
    res = KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(mem, env->target_platform, env_type->target_type, 0, &ir_type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        
        kefir_ir_type_free(mem, &env_type->type);
        KEFIR_FREE(mem, env_type);
        return res;
    });

    env_type->ast_type = type;
    *opaque_type = env_type;
    return KEFIR_OK;
}

static kefir_result_t target_env_free_type(struct kefir_mem *mem,
                                         const struct kefir_ast_target_environment *target_env,
                                         kefir_ast_target_environment_opaque_type_t opaque_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(target_env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST target environment"));
    REQUIRE(opaque_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment type"));
    ASSIGN_DECL_CAST(const struct kefir_ast_translator_environment *, env,
        target_env->payload);

    ASSIGN_DECL_CAST(struct kefir_ast_translator_environment_type *, env_type,
        opaque_type);
    REQUIRE_OK(KEFIR_IR_TARGET_PLATFORM_FREE_TYPE(mem, env->target_platform, env_type->target_type));
    REQUIRE_OK(kefir_ast_type_layout_free(mem, env_type->layout));
    env_type->layout = NULL;
    REQUIRE_OK(kefir_ir_type_free(mem, &env_type->type));
    *env_type = (struct kefir_ast_translator_environment_type){0};
    KEFIR_FREE(mem, env_type);
    return KEFIR_OK;
}

static kefir_result_t resolve_member(struct kefir_ast_type_layout *current_layout,
                                   const char *identifier,
                                   struct kefir_ast_type_layout **layout,
                                   kefir_size_t *offset) {
    REQUIRE(current_layout->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
        current_layout->type->tag == KEFIR_AST_TYPE_UNION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected struct/union type to correspond to member designator"));

    struct kefir_hashtree_node *node = NULL;
    kefir_result_t res = kefir_hashtree_at(&current_layout->structure_layout.members,
        (kefir_hashtree_key_t) identifier, &node);
    if (res == KEFIR_OK) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, next_layout,
            node->value);
        *offset += next_layout->properties.relative_offset;
        *layout = next_layout;
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        for (const struct kefir_list_entry *iter = kefir_list_head(&current_layout->structure_layout.anonymous_members);
            iter != NULL && res == KEFIR_NOT_FOUND;
            kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, anon_layout,
                iter->value);
            res = resolve_member(anon_layout, identifier, layout, offset);
            if (res == KEFIR_OK) {
                *offset += anon_layout->properties.relative_offset;
            }
        }
    }
    return res;
}

static kefir_result_t resolve_subscript(struct kefir_ast_type_layout *current_layout,
                                      kefir_size_t index,
                                      struct kefir_ast_type_layout **layout,
                                      kefir_size_t *offset) {
    REQUIRE(current_layout->type->tag == KEFIR_AST_TYPE_ARRAY,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected array type to resolve subscript"));
    struct kefir_ast_type_layout *next_layout = current_layout->array_layout.element_type;
    *layout = next_layout;
    *offset += next_layout->properties.relative_offset + 
        index * next_layout->properties.size;
    return KEFIR_OK;
}

static kefir_result_t resolve_layout(struct kefir_ast_type_layout *root,
                                   const struct kefir_ast_designator *designator,
                                   struct kefir_ast_type_layout **layout,
                                   kefir_size_t *offset) {
    struct kefir_ast_type_layout *current_layout = NULL;
    if (designator->next != NULL) {
        REQUIRE_OK(resolve_layout(root, designator->next, &current_layout, offset));
    } else {
        current_layout = root;
        *offset += root->properties.relative_offset;
    }

    if (designator->type == KEFIR_AST_DESIGNATOR_MEMBER) {
        REQUIRE_OK(resolve_member(current_layout, designator->member, layout, offset));
    } else {
        REQUIRE_OK(resolve_subscript(current_layout, designator->index, layout, offset));
    }
    return KEFIR_OK;
}

static kefir_result_t target_env_object_info(struct kefir_mem *mem,
                                         const struct kefir_ast_target_environment *env,
                                         kefir_ast_target_environment_opaque_type_t target_type,
                                         const struct kefir_ast_designator *designator,
                                         struct kefir_ast_target_environment_object_info *object_info) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment"));
    REQUIRE(target_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid target type"));
    REQUIRE(object_info != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST object info pointer"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_environment_type *, type,
        target_type);
    if (designator != NULL) {
        *object_info = (struct kefir_ast_target_environment_object_info){0};
        struct kefir_ast_type_layout *final_layout = NULL;
        REQUIRE_OK(resolve_layout(type->layout, designator, &final_layout, &object_info->relative_offset));
        object_info->size = final_layout->properties.size;
        object_info->aligned = final_layout->properties.aligned;
        object_info->alignment = final_layout->properties.alignment;
    } else {
        object_info->size = type->layout->properties.size;
        object_info->aligned = type->layout->properties.aligned;
        object_info->alignment = type->layout->properties.alignment;
        object_info->relative_offset = type->layout->properties.relative_offset;
    }
    
    return KEFIR_OK;
}

static kefir_result_t target_env_object_offset(struct kefir_mem *mem,
                                             const struct kefir_ast_target_environment *env,
                                             kefir_ast_target_environment_opaque_type_t target_type,
                                             kefir_int64_t index,
                                             kefir_int64_t *offset) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment"));
    REQUIRE(target_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid target type"));
    REQUIRE(offset != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid offset pointer"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_environment_type *, type,
        target_type);
    *offset = type->layout->properties.size * index;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_environment_init(struct kefir_ast_translator_environment *env,
                                                 struct kefir_ir_target_platform *target_platform) {    
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid AST translator environment"));
    REQUIRE(target_platform != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expectd valid IR target platform"));
    env->target_platform = target_platform;
    env->target_env.get_type = target_env_get_type;
    env->target_env.free_type = target_env_free_type;
    env->target_env.object_info = target_env_object_info;
    env->target_env.object_offset = target_env_object_offset;
    env->target_env.payload = env;
    return KEFIR_OK;
}
