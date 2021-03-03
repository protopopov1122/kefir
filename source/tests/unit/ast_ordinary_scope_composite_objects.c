#include "kefir/test/unit_test.h"
#include "kefir/ast/context.h"

DEFINE_CASE(ast_ordinary_scope_composite_objects1, "AST ordinary scope - composite external objects")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *array1_type1 = kefir_ast_type_unbounded_array(
            &kft_mem, &global_context.type_bundle, kefir_ast_type_char(), NULL);

    const struct kefir_ast_type *array1_type2 = kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_char(), 100, &(const struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            });

    const struct kefir_ast_type *array1_composite = kefir_ast_type_array(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_char(), 100, NULL);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "array1",
        array1_type1, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "array1", &scoped_id, NULL));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, array1_type1));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "array1",
        array1_type2, NULL));

    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "array1", &scoped_id, NULL));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, array1_composite));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects2, "AST ordinary scope - composite thread local external objects")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *func1_type1 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function1));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "x", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "y", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "z", NULL, NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *func1_type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function2));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_double(), NULL));

    const struct kefir_ast_type *func1_composite = func1_type2;

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "func1",
        func1_type1, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "func1", &scoped_id, NULL));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "func1",
        func1_type2, NULL));

    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "func1", &scoped_id, NULL));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_composite));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

// TODO: Implement composite object type tests