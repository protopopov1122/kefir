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
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int()));
    ASSERT(kefir_hashtree_has(&global_context.external_object_declarations, (kefir_hashtree_key_t) "ext_int1"));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_identifier_flat_scope_at(&global_context.object_scope, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(scoped_id->object.type, kefir_ast_type_signed_int()));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
