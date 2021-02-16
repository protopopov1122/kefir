#include "kefir/test/unit_test.h"
#include "kefir/ast/context.h"

DEFINE_CASE(ast_scope_rules1, "AST Declaration scoping - rules #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules2, "AST Declaration scoping - rules #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules3, "AST Declaration scoping - rules #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules4, "AST Declaration scoping - rules #4")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules5, "AST Declaration scoping - rules #5")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "static_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules6, "AST Declaration scoping - rules #6")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int()));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "static_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
