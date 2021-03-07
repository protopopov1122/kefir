#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/context.h"

DEFINE_CASE(ast_type_scope1, "AST Declaration scoping - tagged type scoping")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_global_context_define_tag(&kft_mem, &global_context, kefir_ast_type_incomplete_structure(
        &kft_mem, &type_bundle, "struct1")));
    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
    ASSERT(!scoped_id->type->structure_type.complete);

    ASSERT_OK(kefir_ast_global_context_define_tag(&kft_mem, &global_context, kefir_ast_type_incomplete_structure(
        &kft_mem, &type_bundle, "struct2")));
    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct2") == 0);
    ASSERT(!scoped_id->type->structure_type.complete);

    ASSERT(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct3", &scoped_id) == KEFIR_NOT_FOUND);

    struct kefir_ast_struct_type *struct_type1 = NULL;
    ASSERT_OK(kefir_ast_global_context_define_tag(&kft_mem, &global_context, kefir_ast_type_structure(
        &kft_mem, &type_bundle, "struct1", &struct_type1)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field1", kefir_ast_type_signed_int(), NULL));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == struct_type1);

    ASSERT_OK(kefir_ast_global_context_define_tag(&kft_mem, &global_context, kefir_ast_type_incomplete_structure(
        &kft_mem, &type_bundle, "struct1")));
    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == struct_type1);

    ASSERT_NOK(kefir_ast_global_context_define_tag(&kft_mem, &global_context, kefir_ast_type_structure(
        &kft_mem, &type_bundle, "struct1", &struct_type1)));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_type_scope2, "AST Declaration scoping - tagged type scoping #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    ASSERT_OK(kefir_ast_global_context_define_tag(&kft_mem, &global_context, kefir_ast_type_structure(
        &kft_mem, &type_bundle, "type1", &struct_type1)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field1", kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field2", kefir_ast_type_unsigned_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field3", kefir_ast_type_pointer(&kft_mem,
        &type_bundle, kefir_ast_type_void()), NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "type1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == struct_type1);

    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_union(
        &kft_mem, &type_bundle, "type1")));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "type1") == 0);
    ASSERT(!scoped_id->type->structure_type.complete);

    struct kefir_ast_struct_type *union_type1 = NULL;
    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_union(
        &kft_mem, &type_bundle, "type1", &union_type1)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field1", kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field2", kefir_ast_type_unsigned_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field3", kefir_ast_type_pointer(&kft_mem,
        &type_bundle, kefir_ast_type_void()), NULL));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "type1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == union_type1);

    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "type1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == union_type1);

    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_enumeration(
        &kft_mem, &type_bundle, "type1")));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "type1") == 0);
    ASSERT(!scoped_id->type->structure_type.complete);

    struct kefir_ast_enum_type *enum_type1 = NULL;
    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_enumeration(
        &kft_mem, &type_bundle, "type1", &enum_type1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, &symbols, enum_type1, "field1"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, &symbols, enum_type1, "field2"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, &symbols, enum_type1, "field3"));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
    ASSERT(strcmp(scoped_id->type->enumeration_type.identifier, "type1") == 0);
    ASSERT(scoped_id->type->enumeration_type.complete);
    ASSERT(&scoped_id->type->enumeration_type == enum_type1);

    ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
    ASSERT(strcmp(scoped_id->type->enumeration_type.identifier, "type1") == 0);
    ASSERT(scoped_id->type->enumeration_type.complete);
    ASSERT(&scoped_id->type->enumeration_type == enum_type1);

    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_context_pop_block_scope(&context));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "type1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == union_type1);

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_type_scope3, "AST Declaration scoping - tagged type scoping #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_structure(
        &kft_mem, &type_bundle, "struct1")));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
    ASSERT(!scoped_id->type->structure_type.complete);

    ASSERT_NOK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_union(
        &kft_mem, &type_bundle, "struct1")));
    ASSERT_NOK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_enumeration(
        &kft_mem, &type_bundle, "struct1")));
    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_structure(
        &kft_mem, &type_bundle, "struct1")));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_structure(
        &kft_mem, &type_bundle, "struct1", &struct_type1)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field1", kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field2", kefir_ast_type_unsigned_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type1, "field3", kefir_ast_type_pointer(&kft_mem,
        &type_bundle, kefir_ast_type_void()), NULL));

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == struct_type1);

    do {
        ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

        ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
        ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
        ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
        ASSERT(scoped_id->type->structure_type.complete);
        ASSERT(&scoped_id->type->structure_type == struct_type1);

        ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_union(
            &kft_mem, &type_bundle, "union1")));

        ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "union1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
        ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
        ASSERT(strcmp(scoped_id->type->structure_type.identifier, "union1") == 0);
        ASSERT(!scoped_id->type->structure_type.complete);

        ASSERT_NOK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_enumeration(
            &kft_mem, &type_bundle, "union1")));
        ASSERT_NOK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_structure(
            &kft_mem, &type_bundle, "union1")));
        ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_union(
            &kft_mem, &type_bundle, "union1")));

        struct kefir_ast_struct_type *union_type1 = NULL;
        ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_union(
            &kft_mem, &type_bundle, "union1", &union_type1)));
        ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field1", kefir_ast_type_double(), NULL));
        ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field2", kefir_ast_type_unsigned_char(), NULL));
        ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field3", kefir_ast_type_pointer(&kft_mem,
            &type_bundle, kefir_ast_type_void()), NULL));

        ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "union1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
        ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
        ASSERT(strcmp(scoped_id->type->structure_type.identifier, "union1") == 0);
        ASSERT(scoped_id->type->structure_type.complete);
        ASSERT(&scoped_id->type->structure_type == union_type1);

        do {
            ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));
            
            ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
            ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
            ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
            ASSERT(scoped_id->type->structure_type.complete);
            ASSERT(&scoped_id->type->structure_type == struct_type1);

            ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "union1", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
            ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
            ASSERT(strcmp(scoped_id->type->structure_type.identifier, "union1") == 0);
            ASSERT(scoped_id->type->structure_type.complete);
            ASSERT(&scoped_id->type->structure_type == union_type1);

            ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_enumeration(
                &kft_mem, &type_bundle, "enum1")));

            ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "enum1", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
            ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
            ASSERT(strcmp(scoped_id->type->enumeration_type.identifier, "enum1") == 0);
            ASSERT(!scoped_id->type->enumeration_type.complete);

            ASSERT_NOK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_structure(
                &kft_mem, &type_bundle, "enum1")));
            ASSERT_NOK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_union(
                &kft_mem, &type_bundle, "enum1")));
            ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_enumeration(
                &kft_mem, &type_bundle, "enum1")));

            struct kefir_ast_enum_type *enum_type1 = NULL;
            ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_enumeration(
                &kft_mem, &type_bundle, "enum1", &enum_type1)));
            ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, &symbols, enum_type1, "field1"));
            ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, &symbols, enum_type1, "field2"));
            ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, &symbols, enum_type1, "field3"));

            ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "enum1", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
            ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
            ASSERT(strcmp(scoped_id->type->enumeration_type.identifier, "enum1") == 0);
            ASSERT(scoped_id->type->enumeration_type.complete);
            ASSERT(&scoped_id->type->enumeration_type == enum_type1);

            do {
                ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

                ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_structure(
                    &kft_mem, &type_bundle, "enum1")));
                ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_union(
                    &kft_mem, &type_bundle, "struct1")));
                ASSERT_OK(kefir_ast_context_define_tag(&kft_mem, &context, kefir_ast_type_incomplete_enumeration(
                    &kft_mem, &type_bundle, "union1")));

                ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "enum1", &scoped_id));
                ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
                ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
                ASSERT(strcmp(scoped_id->type->structure_type.identifier, "enum1") == 0);
                ASSERT(!scoped_id->type->structure_type.complete);

                ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
                ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
                ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
                ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
                ASSERT(!scoped_id->type->structure_type.complete);

                ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "union1", &scoped_id));
                ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
                ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
                ASSERT(strcmp(scoped_id->type->enumeration_type.identifier, "union1") == 0);
                ASSERT(!scoped_id->type->enumeration_type.complete);

                ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
            } while (0);

            ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "enum1", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
            ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_ENUMERATION);
            ASSERT(strcmp(scoped_id->type->enumeration_type.identifier, "enum1") == 0);
            ASSERT(scoped_id->type->enumeration_type.complete);
            ASSERT(&scoped_id->type->enumeration_type == enum_type1);

            ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
        } while (0);

        ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "union1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
        ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_UNION);
        ASSERT(strcmp(scoped_id->type->structure_type.identifier, "union1") == 0);
        ASSERT(scoped_id->type->structure_type.complete);
        ASSERT(&scoped_id->type->structure_type == union_type1);

        ASSERT_NOK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "enum1", &scoped_id));
        
        ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "struct1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(strcmp(scoped_id->type->structure_type.identifier, "struct1") == 0);
    ASSERT(scoped_id->type->structure_type.complete);
    ASSERT(&scoped_id->type->structure_type == struct_type1);

    ASSERT_NOK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "union1", &scoped_id));
    ASSERT_NOK(kefir_ast_context_resolve_scoped_tag_identifier(&context, "enum1", &scoped_id));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_ordinary_constant_scope1, "AST ordinary scope - constant scoping #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c1",
        kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c2",
        kefir_ast_constant_expression_integer(&kft_mem, 2)));
    ASSERT_OK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c3",
        kefir_ast_constant_expression_integer(&kft_mem, 3)));

    struct kefir_ast_constant_expression *cexpr = kefir_ast_constant_expression_integer(&kft_mem, 10);
    ASSERT_NOK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c1",
        cexpr));
    ASSERT_NOK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c2",
        cexpr));
    ASSERT_NOK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c3",
        cexpr));
    ASSERT_OK(kefir_ast_constant_expression_free(&kft_mem, cexpr));

    ASSERT_OK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "c4",
        kefir_ast_constant_expression_integer(&kft_mem, 40)));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 1);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 2);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 3);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c4", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 40);

    do {
        ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 1);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c2", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 2);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c3", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 3);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c4", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 40);

        ASSERT_OK(kefir_ast_context_define_constant(&kft_mem, &context, "c1",
            kefir_ast_constant_expression_integer(&kft_mem, 10)));
        ASSERT_OK(kefir_ast_context_define_constant(&kft_mem, &context, "c2",
            kefir_ast_constant_expression_integer(&kft_mem, 20)));
        ASSERT_OK(kefir_ast_context_define_constant(&kft_mem, &context, "c5",
            kefir_ast_constant_expression_integer(&kft_mem, 50)));

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 10);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c2", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 20);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c3", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 3);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c4", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 40);

        ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c5", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
        ASSERT(scoped_id->enum_constant->value == 50);

        do {
            ASSERT_OK(kefir_ast_context_push_block_scope(&kft_mem, &context));

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c3", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
            ASSERT(scoped_id->enum_constant->value == 3);

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c4", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
            ASSERT(scoped_id->enum_constant->value == 40);

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c5", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
            ASSERT(scoped_id->enum_constant->value == 50);

            ASSERT_OK(kefir_ast_context_define_auto(&kft_mem, &context, "c3",
                kefir_ast_type_unsigned_char(), NULL));

            struct kefir_ast_constant_expression *cexpr = kefir_ast_constant_expression_integer(&kft_mem, 300);
            ASSERT_NOK(kefir_ast_context_define_constant(&kft_mem, &context, "c3",
                cexpr));
            ASSERT_OK(kefir_ast_constant_expression_free(&kft_mem, cexpr));

            ASSERT_OK(kefir_ast_context_define_constant(&kft_mem, &context, "c5",
                kefir_ast_constant_expression_integer(&kft_mem, 500)));

            ASSERT_NOK(kefir_ast_context_define_register(&kft_mem, &context, "c5",
                kefir_ast_type_unsigned_char(), NULL));

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c3", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c4", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
            ASSERT(scoped_id->enum_constant->value == 40);

            ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c5", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
            ASSERT(scoped_id->enum_constant->value == 500);

            ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
        } while (0);

        struct kefir_ast_constant_expression *cexpr = kefir_ast_constant_expression_integer(&kft_mem, 100);
        ASSERT_NOK(kefir_ast_context_define_constant(&kft_mem, &context, "c1",
            cexpr));
        ASSERT_NOK(kefir_ast_context_define_constant(&kft_mem, &context, "c2",
            cexpr));
        ASSERT_NOK(kefir_ast_context_define_constant(&kft_mem, &context, "c5",
            cexpr));
        ASSERT_OK(kefir_ast_constant_expression_free(&kft_mem, cexpr));

        ASSERT_OK(kefir_ast_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 1);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 2);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 3);

    ASSERT_OK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c4", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
    ASSERT(scoped_id->enum_constant->value == 40);

    ASSERT_NOK(kefir_ast_context_resolve_scoped_ordinary_identifier(&context, "c5", &scoped_id));

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE