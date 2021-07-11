#include "kefir/ast-translator/context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_context_init(struct kefir_ast_translator_context *context,
                                                 const struct kefir_ast_context *ast_context,
                                                 const struct kefir_ast_translator_environment *environment,
                                                 struct kefir_ir_module *module) {
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected a pointer to valid AST translator context"));
    REQUIRE(ast_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(environment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));

    REQUIRE_OK(kefir_ast_translator_type_cache_init(&context->type_cache, NULL));
    context->base_context = NULL;
    context->ast_context = ast_context;
    context->environment = environment;
    context->module = module;
    context->global_scope_layout = NULL;
    context->local_scope_layout = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_context_init_local(struct kefir_ast_translator_context *context,
                                                       const struct kefir_ast_context *ast_context,
                                                       struct kefir_ast_translator_context *base_context) {
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected a pointer to valid AST translator context"));
    REQUIRE(ast_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(base_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid base AST translator context"));

    REQUIRE_OK(kefir_ast_translator_type_cache_init(&context->type_cache, &base_context->type_cache.resolver));
    context->base_context = base_context;
    context->ast_context = ast_context;
    context->environment = base_context->environment;
    context->module = base_context->module;
    context->global_scope_layout = NULL;
    context->local_scope_layout = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_context_free(struct kefir_mem *mem, struct kefir_ast_translator_context *context) {
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected a pointer to valid AST translator context"));

    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_FREE(mem, &context->type_cache.resolver));
    context->base_context = NULL;
    context->ast_context = NULL;
    context->environment = NULL;
    context->module = NULL;
    context->global_scope_layout = NULL;
    context->local_scope_layout = NULL;
    return KEFIR_OK;
}

struct kefir_ast_translator_type_resolver *kefir_ast_translator_context_type_resolver(
    struct kefir_ast_translator_context *context) {
    REQUIRE(context != NULL, NULL);
    return &context->type_cache.resolver;
}
