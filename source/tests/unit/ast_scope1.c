#include "kefir/test/unit_test.h"
#include "kefir/ast/context.h"

#define ASSERT_RESOLVED_IDENTIFIER(_context, _identifier, _storage, _type, _linkage) \
    do { \
        const struct kefir_ast_scoped_identifier *__scoped_id = NULL; \
        kefir_ast_scoped_identifier_linkage_t __linkage; \
        ASSERT_OK(kefir_ast_context_resolve_object_identifier((_context), (_identifier), &__scoped_id, &__linkage)); \
        ASSERT(__scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT); \
        ASSERT(__scoped_id->object.storage == (_storage)); \
        ASSERT(KEFIR_AST_TYPE_COMPATIBLE((_context)->global->type_traits, __scoped_id->object.type, (_type))); \
        ASSERT(__linkage == (_linkage)); \
    } while (0)

DEFINE_CASE(ast_scope_rules1, "AST Declaration scoping - global rules for extern")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules2, "AST Declaration scoping - global rules for thread_local extern")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules3, "AST Declaration scoping - global rules for extern #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules4, "AST Declaration scoping - global rules for thread_local extern #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules5, "AST Declaration scoping - global rules for static")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "static_int1"));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "static_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int3", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int3", kefir_ast_type_signed_int(), NULL));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules6, "AST Declaration scoping - global rules for thread_local static")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "static_int1"));

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "static_int3", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int3", kefir_ast_type_signed_int(), NULL));
    
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "static_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules7, "AST Declaration scoping - local extern")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
        
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules8, "AST Declaration scoping - local extern thread_local")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL));
        
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "ext_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules9, "AST Declaration scoping - local static")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
        
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "static_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "static_int1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules10, "AST Declaration scoping - local static thread_local")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_declare_external_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(), NULL));
        
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "static_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "static_int1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules11, "AST Declaration scoping - local auto")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int3", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
        
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "auto_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "auto_int1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules12, "AST Declaration scoping - local register")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_static_thread_local(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_NOK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_define_auto(&kft_mem, &context, "auto_int3", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
        
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_ast_scoped_identifier_linkage_t linkage;
    ASSERT_OK(kefir_ast_context_resolve_object_identifier(&context, "auto_int1", &scoped_id, &linkage));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "auto_int1"));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_scope_rules13, "AST Declaration scoping - block scoping")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external(
        &kft_mem, &global_context, "variable1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(
        &kft_mem, &global_context, "variable2", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(
        &kft_mem, &global_context, "variable1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(
        &kft_mem, &global_context, "variable3", kefir_ast_type_bool(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(
        &kft_mem, &global_context, "variable4", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(
        &kft_mem, &global_context, "variable4", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "variable5", kefir_ast_type_signed_int(), NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
        kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable3", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        kefir_ast_type_bool(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable4", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable5", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_OK(kefir_ast_context_declare_external(
        &kft_mem, &context, "variable1", kefir_ast_type_signed_int(), NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
        kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_OK(kefir_ast_context_define_static(
        &kft_mem, &context, "variable2", kefir_ast_type_float(), NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
        kefir_ast_type_float(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_context_define_auto(
        &kft_mem, &context, "variable10", kefir_ast_type_unsigned_int(), NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable10", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
        kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_context_define_register(
        &kft_mem, &context, "variable10", kefir_ast_type_unsigned_int(), NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable10", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
        kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_context_define_static_thread_local(
        &kft_mem, &context, "variable2", kefir_ast_type_unsigned_int(), NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
        kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));

    ASSERT_RESOLVED_IDENTIFIER(&context, "variable10", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
        kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));

    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
        kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable3", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        kefir_ast_type_bool(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable4", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable5", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
        kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    
    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE