#include "kefir/ast-translator/context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_context_init(struct kefir_ast_translator_context *context,
                                             const struct kefir_ast_context *ast_context,
                                             const struct kefir_ast_translator_environment *environment,
                                             struct kefir_ir_module *module) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected a pointer to valid AST translator context"));
    REQUIRE(ast_context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(environment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));

    context->ast_context = ast_context;
    context->environment = environment;
    context->module = module;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_context_free(struct kefir_ast_translator_context *context) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected a pointer to valid AST translator context"));
    context->ast_context = NULL;
    context->environment = NULL;
    context->module = NULL;
    return KEFIR_OK;
}