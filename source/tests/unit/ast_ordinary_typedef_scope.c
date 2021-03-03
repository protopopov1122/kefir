#include "kefir/test/unit_test.h"
#include "kefir/ast/context.h"

DEFINE_CASE(ast_ordinary_typedef_scope1, "AST ordinary scope - type definitions #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type1",
        kefir_ast_type_void()));
    ASSERT_OK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type2",
        kefir_ast_type_signed_long()));
    ASSERT_OK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type3",
        kefir_ast_type_float()));

    ASSERT_NOK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type1",
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void())));
    ASSERT_OK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type2",
        kefir_ast_type_signed_long()));
    ASSERT_NOK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type3",
        kefir_ast_type_double()));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "val1",
        kefir_ast_type_unsigned_char(), NULL));
    ASSERT_NOK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "val1",
        kefir_ast_type_unsigned_char()));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_void()));

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_signed_long()));

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_float()));

    ASSERT_NOK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type4", &scoped_id));

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "val1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

    do {
        ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

        ASSERT_OK(kefir_ast_context_define_static(&kft_mem, &context, "type1",
            kefir_ast_type_unsigned_char(), NULL));
            
        ASSERT_OK(kefir_ast_context_define_type(&kft_mem, &context, "val1",
            kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long(), NULL)));

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "val1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type,
            kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long(), NULL)));

        do {
            ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

            ASSERT_OK(kefir_ast_context_define_type(&kft_mem, &context, "type2",
                kefir_ast_type_qualified(&kft_mem, &type_bundle,
                    kefir_ast_type_bool(), (const struct kefir_ast_type_qualification){
                    .constant = false,
                    .restricted = true,
                    .volatile_type = false
                })));

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
            ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_qualified(&kft_mem, &type_bundle,
                kefir_ast_type_bool(), (const struct kefir_ast_type_qualification){
                    .constant = false,
                    .restricted = true,
                    .volatile_type = false
                })));

            ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
        } while (0);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_signed_long()));

        ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_void()));

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_signed_long()));

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_float()));

    ASSERT_NOK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "type4", &scoped_id));

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "val1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE