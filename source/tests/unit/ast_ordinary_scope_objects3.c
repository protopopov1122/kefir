#include "kefir/test/unit_test.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/node.h"
#include "kefir/ast/type.h"
#include "kefir/test/util.h"

#define ASSERT_RESOLVED_IDENTIFIER(_context, _identifier, _storage, _type, _linkage) \
    do { \
        const struct kefir_ast_scoped_identifier *__scoped_id = NULL; \
        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier((_context), (_identifier), &__scoped_id)); \
        ASSERT(__scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT); \
        ASSERT(__scoped_id->object.storage == (_storage)); \
        ASSERT(KEFIR_AST_TYPE_SAME(__scoped_id->object.type, (_type))); \
        ASSERT(__scoped_id->object.linkage == (_linkage)); \
    } while (0)

DEFINE_CASE(ast_ordinary_scope_objects_local_init1, "AST Declaration initializers - local static")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_float(), NULL);
    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_float(), kefir_ast_constant_expression_integer(&kft_mem, 3), NULL);

    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var1", kefir_ast_type_signed_int(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var1", kefir_ast_type_signed_int(), NULL, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 =
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var2", kefir_ast_type_signed_char(), NULL, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var2",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, kefir_ast_type_signed_char(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var2", kefir_ast_type_signed_char(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var2",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, kefir_ast_type_signed_char(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var3", type1, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var3", type2, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init3 =
        kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var3", type2, NULL, init3));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init4 =
        kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init4->list,
        kefir_ast_new_index_desginator(&kft_mem, 2, NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f)))));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var4", type1, NULL, init4));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var4",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context,
        "var4", type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_local_init2, "AST Declaration initializers - local thread local static")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_float(), NULL);
    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_float(), kefir_ast_constant_expression_integer(&kft_mem, 3), NULL);

    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var1", kefir_ast_type_signed_int(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var1", kefir_ast_type_signed_int(), NULL, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 =
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')));
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var2", kefir_ast_type_signed_char(), NULL, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var2",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, kefir_ast_type_signed_char(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var2", kefir_ast_type_signed_char(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var2",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, kefir_ast_type_signed_char(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var3", type1, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var3", type2, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init3 =
        kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var3", type2, NULL, init3));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init4 =
        kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init4->list,
        kefir_ast_new_index_desginator(&kft_mem, 2, NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f)))));
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var4", type1, NULL, init4));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var4",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context,
        "var4", type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_local_init3, "AST Declaration initializers - local auto")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_signed_char(), NULL);
    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_signed_char(), kefir_ast_constant_expression_integer(&kft_mem, 5), NULL);

    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var1", kefir_ast_type_signed_short(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, kefir_ast_type_signed_short(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var1", kefir_ast_type_signed_short(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, kefir_ast_type_signed_short(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 60)));
    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var1", kefir_ast_type_signed_short(), NULL, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, kefir_ast_type_signed_short(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 60)))));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var2", kefir_ast_type_double(), NULL, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var2",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, kefir_ast_type_double(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var3", type1, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var3", type2, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var3", type2, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init3 = kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hey!")));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context,
        "var4", type1, NULL, init3));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var4",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_local_init4, "AST Declaration initializers - local register")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_signed_char(), NULL);
    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context.context.type_bundle,
        kefir_ast_type_signed_char(), kefir_ast_constant_expression_integer(&kft_mem, 5), NULL);

    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var1", kefir_ast_type_signed_short(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, kefir_ast_type_signed_short(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var1", kefir_ast_type_signed_short(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, kefir_ast_type_signed_short(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 60)));
    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var1", kefir_ast_type_signed_short(), NULL, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, kefir_ast_type_signed_short(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init2->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 60)))));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var2", kefir_ast_type_double(), NULL, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var2",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, kefir_ast_type_double(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var3", type1, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var3", type2, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var3", type2, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var3",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    struct kefir_ast_initializer *init3 = kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hey!")));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context,
        "var4", type1, NULL, init3));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var4",
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, type2, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));
END_CASE