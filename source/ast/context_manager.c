#include "kefir/ast/context_manager.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_context_manager_init(struct kefir_mem *mem,
                                          const struct kefir_ast_type_traits *type_traits,
                                          struct kefir_ast_target_environment *env,
                                          struct kefir_ast_context_manager *context_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment"));
    REQUIRE(context_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));

    context_manager->global = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_global_context));
    REQUIRE(context_manager->global != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST global context"));
    kefir_result_t res = kefir_ast_global_context_init(mem, type_traits, env, context_manager->global);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, context_manager->global);
        return res;
    });

    context_manager->local = NULL;
    context_manager->current = &context_manager->global->context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_manager_free(struct kefir_mem *mem,
                                          struct kefir_ast_context_manager *context_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));
    
    if (context_manager->local != NULL) {
        REQUIRE_OK(kefir_ast_local_context_free(mem, context_manager->local));
        KEFIR_FREE(mem, context_manager->local);
        context_manager->local = NULL;
    }
    REQUIRE_OK(kefir_ast_global_context_free(mem, context_manager->global));
    KEFIR_FREE(mem, context_manager->global);
    context_manager->global = NULL;
    context_manager->current = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_manager_open_local(struct kefir_mem *mem,
                                                struct kefir_ast_context_manager *context_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));
    REQUIRE(context_manager->local == NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Local context is already open"));

    context_manager->local = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_local_context));
    REQUIRE(context_manager->local != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate local AST context"));
    kefir_result_t res = kefir_ast_local_context_init(mem, context_manager->global, context_manager->local);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, context_manager->local);
        return res;
    });

    context_manager->current = &context_manager->local->context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_manager_close_local(struct kefir_mem *mem,
                                                 struct kefir_ast_context_manager *context_manager) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context_manager != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context manager"));
    REQUIRE(context_manager->local != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Local context is not open"));
    
    REQUIRE_OK(kefir_ast_local_context_free(mem, context_manager->local));
    KEFIR_FREE(mem, context_manager->local);
    context_manager->local = NULL;
    context_manager->current = &context_manager->global->context;
    return KEFIR_OK;
}