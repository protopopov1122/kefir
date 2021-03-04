#include "kefir/test/unit_test.h"
#include "kefir/ast/context.h"

DEFINE_CASE(ast_ordinary_function_scope1, "AST ordinary scope - functions #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_void(), "", &function_type1);
    
    ASSERT_NOK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type1));

    type1 = kefir_ast_type_function(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_void(), "func1", &function_type1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type1,
        "x", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type1,
        "y", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type1,
        "z", NULL, NULL));
    
    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type1));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(kefir_hashtree_has(&global_context.external_function_declarations, (kefir_hashtree_key_t) "func1"));

    struct kefir_ast_function_type *function_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_signed_int(), "func1", &function_type2);

    ASSERT_NOK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type2));

    type2 = kefir_ast_type_function(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_void(), "func1", &function_type2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type2,
        "x", kefir_ast_type_signed_long_long(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type2,
        "y", kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type2,
        "z", kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), NULL));

    ASSERT_OK(kefir_ast_global_context_define_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
        type2));
    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->function.type, type2));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(!kefir_hashtree_has(&global_context.external_function_declarations, (kefir_hashtree_key_t) "func1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

// TODO Define more unit tests for function scoping