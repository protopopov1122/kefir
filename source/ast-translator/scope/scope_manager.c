#include "kefir/ast-translator/scope/scope_manager.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_scope_manager_init(struct kefir_mem *mem,
                                                   struct kefir_ast_translator_context *context,
                                                   struct kefir_ast_global_context *global_context,
                                                   struct kefir_ast_translator_scope_manager *scope_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(scope_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator scope manager"));

    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_push(mem, &context->type_resolver));

    scope_manager->global_layout = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_global_scope_layout));
    REQUIRE_ELSE(scope_manager->global_layout != NULL, {
        kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate global AST translator scope layout");
    });

    kefir_result_t res = kefir_ast_translator_global_scope_layout_init(mem,
        context->module, scope_manager->global_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, scope_manager->global_layout);
        kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver);
        return res;
    });

    res = kefir_ast_translator_build_global_scope_layout(mem, context->module,
        global_context, context->environment, &context->type_resolver.resolver,
        scope_manager->global_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_global_scope_layout_free(mem, scope_manager->global_layout);
        KEFIR_FREE(mem, scope_manager->global_layout);
        kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver);
        return res;
    });

    scope_manager->local_layout = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_scope_manager_free(struct kefir_mem *mem,
                                                   struct kefir_ast_translator_context *context,
                                                   struct kefir_ast_translator_scope_manager *scope_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(scope_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator scope manager"));
    
    if (scope_manager->local_layout != NULL) {
        REQUIRE_OK(kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver));
        REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, scope_manager->local_layout));
        KEFIR_FREE(mem, scope_manager->local_layout);
        scope_manager->local_layout = NULL;
    }

    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, scope_manager->global_layout));
    KEFIR_FREE(mem, scope_manager->global_layout);
    scope_manager->global_layout = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_scope_manager_open_local(struct kefir_mem *mem,
                                                         struct kefir_ast_translator_context *context,
                                                         struct kefir_ast_local_context *local_context,
                                                         struct kefir_ast_translator_scope_manager *scope_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(local_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local context"));
    REQUIRE(scope_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator scope manager"));
    REQUIRE(scope_manager->local_layout == NULL,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "AST translator scope manager already has local scope"));

    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_push(mem, &context->type_resolver));
    
    scope_manager->local_layout = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_local_scope_layout));
    REQUIRE_ELSE(scope_manager->local_layout != NULL, {
        kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate local AST translator scope layout");
    });
    
    kefir_result_t res = kefir_ast_translator_local_scope_layout_init(mem, context->module,
        scope_manager->global_layout, scope_manager->local_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, scope_manager->local_layout);
        scope_manager->local_layout = NULL;
        kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver);
        return res;
    });

    res = kefir_ast_translator_build_local_scope_layout(mem, local_context, context->environment,
        context->module, &context->type_resolver.resolver, scope_manager->local_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, scope_manager->local_layout);
        KEFIR_FREE(mem, scope_manager->local_layout);
        scope_manager->local_layout = NULL;
        kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_scope_manager_close_local(struct kefir_mem *mem,
                                                          struct kefir_ast_translator_context *context,
                                                          struct kefir_ast_translator_scope_manager *scope_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(scope_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator scope manager"));
    REQUIRE(scope_manager->local_layout != NULL,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "AST translator scope manager has no local scope"));

    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_pop(mem, &context->type_resolver));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, scope_manager->local_layout));
    KEFIR_FREE(mem, scope_manager->local_layout);
    scope_manager->local_layout = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_scope_manager_define_globals(struct kefir_mem *mem,
                                                             struct kefir_ast_translator_context *context,
                                                             struct kefir_ast_translator_scope_manager *scope_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(scope_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator scope manager"));
    
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, context->module, scope_manager->global_layout));
    return KEFIR_OK;
}