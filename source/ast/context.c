#include "kefir/ast/context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translation_global_context_init(struct kefir_mem *mem,
                                                     const struct kefir_ast_type_traits *type_traits,
                                                     struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_symbol_table_init(&context->symbols));
    REQUIRE_OK(kefir_ast_type_repository_init(&context->type_repository, &context->symbols));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->object_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translation_global_context_free(struct kefir_mem *mem,
                                                     struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->object_scope));
    REQUIRE_OK(kefir_ast_type_repository_free(mem, &context->type_repository));
    REQUIRE_OK(kefir_symbol_table_free(mem, &context->symbols));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_init(struct kefir_mem *mem,
                                              struct kefir_ast_global_context *global,
                                              struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global translation context"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    context->global = global;
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->local_object_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_free(struct kefir_mem *mem,
                                              struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->local_object_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_resolve_object_identifier(const struct kefir_ast_context *context,
                                                            const char *identifier,
                                                            const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->local_object_scope, identifier, scoped_identifier);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_identifier_flat_scope_at(&context->global->object_scope, identifier, scoped_identifier);
    }
    return res;
}

kefir_result_t kefir_ast_context_declare_local_object_identifier(struct kefir_mem *mem,
                                                                  struct kefir_ast_context *context,
                                                                  const char *identifier,
                                                                  const struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->local_object_scope, identifier, scoped_identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_declare_global_object_identifier(struct kefir_mem *mem,
                                                                   struct kefir_ast_context *context,
                                                                   const char *identifier,
                                                                   const struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier"));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->global->object_scope, identifier, scoped_identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_push_block_scope(struct kefir_mem *mem,
                                                          struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->local_object_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_pop_block_scope(struct kefir_ast_context *context) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->local_object_scope));
    return KEFIR_OK;
}