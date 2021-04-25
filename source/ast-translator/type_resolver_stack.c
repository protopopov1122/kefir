#include "kefir/ast-translator/type_resolver_stack.h"
#include "kefir/ast-translator/type_cache.h"
#include "kefir/core/linked_stack.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t stack_layer_free(struct kefir_mem *mem,
                                     struct kefir_list *list,
                                     struct kefir_list_entry *entry,
                                     void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_cache *, layer,
        entry->value);
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_FREE(mem, &layer->resolver));
    KEFIR_FREE(mem, layer);
    return KEFIR_OK;
}

static kefir_result_t resolver_resolve(const struct kefir_ast_translator_type_resolver *resolver,
                                     const struct kefir_ast_type *type,
                                     kefir_size_t alignment,
                                     const struct kefir_ast_translator_resolved_type **resolved_type) {
    REQUIRE(resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_resolver_stack *, stack,
        resolver->payload);
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_RESOLVE(kefir_ast_translator_type_resolver_stack_top(stack),
        type, alignment, resolved_type));
    return KEFIR_OK;
}

static  kefir_result_t resolver_register_object(struct kefir_mem *mem,
                                              const struct kefir_ast_translator_type_resolver *resolver,
                                              kefir_id_t ir_type_id,
                                              struct kefir_ir_type *ir_type,
                                              struct kefir_ast_type_layout *layout) {
    REQUIRE(resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_resolver_stack *, stack,
        resolver->payload);
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_OBJECT(mem, kefir_ast_translator_type_resolver_stack_top(stack),
        ir_type_id, ir_type, layout));
    return KEFIR_OK;
}

static  kefir_result_t resolver_register_function(struct kefir_mem *mem,
                                                const struct kefir_ast_translator_type_resolver *resolver,
                                                struct kefir_ast_translator_function_declaration *declaration) {
    REQUIRE(resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_resolver_stack *, stack,
        resolver->payload);
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_FUNCTION(mem, kefir_ast_translator_type_resolver_stack_top(stack),
        declaration));
    return KEFIR_OK;
}

static kefir_result_t resolver_build_object(struct kefir_mem *mem,
                                          struct kefir_ast_translator_type_resolver *resolver,
                                          const struct kefir_ast_translator_environment *env,
                                          struct kefir_ir_module *module,
                                          const struct kefir_ast_type *type,
                                          kefir_size_t alignment,
                                          const struct kefir_ast_translator_resolved_type **resolved_type) {
    REQUIRE(resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_resolver_stack *, stack,
        resolver->payload);
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, kefir_ast_translator_type_resolver_stack_top(stack),
        env, module, type, alignment, resolved_type));
    return KEFIR_OK;
}

static kefir_result_t resolver_build_function(struct kefir_mem *mem,
                                          struct kefir_ast_translator_type_resolver *resolver,
                                          const struct kefir_ast_translator_environment *env,
                                          struct kefir_ast_type_bundle *type_bundle,
                                          const struct kefir_ast_type_traits *type_traits,
                                          struct kefir_ir_module *module,
                                          const struct kefir_ast_type *type,
                                          const struct kefir_ast_translator_resolved_type **resolved_type) {
    REQUIRE(resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_resolver_stack *, stack,
        resolver->payload);
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(mem, kefir_ast_translator_type_resolver_stack_top(stack),
        env, type_bundle, type_traits, module, type, resolved_type));
    return KEFIR_OK;
}

static kefir_result_t resolver_free(struct kefir_mem *mem,
                                  struct kefir_ast_translator_type_resolver *resolver) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));

    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_resolver_stack *, stack,
        resolver->payload);
    REQUIRE_OK(kefir_list_free(mem, &stack->stack));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_resolver_stack_init(struct kefir_mem *mem,
                                                         struct kefir_ast_translator_type_resolver_stack *stack) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(stack != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver stack"));

    REQUIRE_OK(kefir_list_init(&stack->stack));
    REQUIRE_OK(kefir_list_on_remove(&stack->stack, stack_layer_free, NULL));
    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_push(mem, stack));

    stack->resolver.resolve = resolver_resolve;
    stack->resolver.register_object = resolver_register_object;
    stack->resolver.register_function = resolver_register_function;
    stack->resolver.build_object = resolver_build_object;
    stack->resolver.build_function = resolver_build_function;
    stack->resolver.free = resolver_free;
    stack->resolver.payload = stack;
    return KEFIR_OK;
}

struct kefir_ast_translator_type_resolver *kefir_ast_translator_type_resolver_stack_top(const struct kefir_ast_translator_type_resolver_stack *stack) {
    REQUIRE(stack != NULL, NULL);
    void *top = NULL;
    REQUIRE(kefir_linked_stack_peek(&stack->stack, &top) == KEFIR_OK, NULL);
    REQUIRE(top != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_translator_type_cache *, layer,
        top);
    return &layer->resolver;
}

kefir_result_t kefir_ast_translator_type_resolver_stack_push(struct kefir_mem *mem,
                                                         struct kefir_ast_translator_type_resolver_stack *stack) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(stack != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver stack"));

    struct kefir_ast_translator_type_cache *layer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_type_cache));
    REQUIRE(layer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Unable to allocate AST translator type cache"));
    kefir_result_t res = kefir_ast_translator_type_cache_init(layer, kefir_ast_translator_type_resolver_stack_top(stack));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, layer);
        return res;
    });
    res = kefir_linked_stack_push(mem, &stack->stack, layer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_FREE(mem, &layer->resolver);
        KEFIR_FREE(mem, layer);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_type_resolver_stack_pop(struct kefir_mem *mem,
                                                        struct kefir_ast_translator_type_resolver_stack *stack) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(stack != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver stack"));
    REQUIRE(kefir_list_length(&stack->stack) > 1, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to pop base resolver"));
    REQUIRE_OK(kefir_linked_stack_pop(mem, &stack->stack, NULL));
    return KEFIR_OK;
}