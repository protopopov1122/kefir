#include "kefir/test/unit_test.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_ordinary_scope_composite_objects_external, "AST ordinary scope - composite external objects")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *array1_type1 = kefir_ast_type_unbounded_array(
            &kft_mem, &global_context.type_bundle, kefir_ast_type_char(), NULL);

    const struct kefir_ast_type *array1_type2 = kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_char(), kefir_ast_constant_expression_integer(&kft_mem, 100),
            &(const struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            });

    const struct kefir_ast_type *array1_composite = kefir_ast_type_array(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_char(), kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "array1",
        array1_type1, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, array1_type1));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "array1",
        array1_type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, array1_composite));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_external_declaration, "AST ordinary scope - composite external object declaration")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *array1_type1 = kefir_ast_type_unbounded_array(
            &kft_mem, &global_context.type_bundle, kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_char()), NULL);

    const struct kefir_ast_type *array1_type2 = kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_char()),
            kefir_ast_constant_expression_integer(&kft_mem, 100),
            &(const struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            });

    const struct kefir_ast_type *array1_composite = kefir_ast_type_array(&kft_mem, &global_context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_char()),
        kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "array1",
        array1_type1, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, array1_type1));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "array1",
        array1_type2, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, array1_composite));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_thread_local_external, "AST ordinary scope - composite thread local external objects")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

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
        func1_type1, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "func1",
        func1_type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_composite));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_thread_local_external_declaration,
    "AST ordinary scope - composite thread local external object declaration")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

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
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_double(), NULL));

    const struct kefir_ast_type *func1_composite = func1_type2;

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "func1",
        func1_type1, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "func1",
        func1_type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_composite));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_static, "AST ordinary scope - composite static object")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *func1_type1 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function1));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
            kefir_ast_constant_expression_integer(&kft_mem, 10), NULL), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(), NULL), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *func1_type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function2));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(), NULL), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(),
            kefir_ast_constant_expression_integer(&kft_mem, 100), NULL), NULL));

    struct kefir_ast_function_type *function3 = NULL;
    const struct kefir_ast_type *func1_type3 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function3));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int()), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_double()), NULL));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "func1",
        func1_type1, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "func1",
        func1_type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_static_thread_local, "AST ordinary scope - composite static thread local object")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *func1_type1 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function1));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(), NULL), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *func1_type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function2));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(),
            kefir_ast_constant_expression_integer(&kft_mem, 100), NULL), NULL));

    struct kefir_ast_function_type *function3 = NULL;
    const struct kefir_ast_type *func1_type3 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function3));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_double()), NULL));

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "func1",
        func1_type1, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "func1",
        func1_type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_local_external, "AST ordinary scope - composite local external object")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function_aux = NULL;
    const struct kefir_ast_type *aux_type =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(), "", &function_aux);

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *func1_type1 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function1));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        aux_type, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(), NULL), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *func1_type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function2));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        aux_type, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(),
            kefir_ast_constant_expression_integer(&kft_mem, 100), NULL), NULL));

    struct kefir_ast_function_type *function3 = NULL;
    const struct kefir_ast_type *func1_type3 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function3));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, aux_type), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_double()), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "func1",
        func1_type1, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "func1",
        func1_type2, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_ordinary_scope_composite_objects_local_thread_local_external,
    "AST ordinary scope - composite local external thread local object")

    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function_aux = NULL;
    const struct kefir_ast_type *aux_type =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(), "", &function_aux);

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *func1_type1 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function1));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        aux_type, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, NULL,
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(), NULL), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *func1_type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function2));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        aux_type, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, NULL,
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_double(),
            kefir_ast_constant_expression_integer(&kft_mem, 100), NULL), NULL));

    struct kefir_ast_function_type *function3 = NULL;
    const struct kefir_ast_type *func1_type3 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function3));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle,
            kefir_ast_type_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, aux_type), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function3, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_double()), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "func1",
        func1_type1, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type1));

    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "func1",
        func1_type2, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, func1_type3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE