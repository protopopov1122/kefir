#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_node_analysis_declarations1, "AST node analysis - declarations #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_declaration *decl1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "some_var1"), NULL);
ASSERT(decl1 != NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(
    &kft_mem, &decl1->specifiers,
    kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16)))));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_extern(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

const struct kefir_ast_type *type1 =
    kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
                             (struct kefir_ast_type_qualification){.constant = true});
ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "some_var1") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 16);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "some_var1", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
ASSERT(scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 16);
ASSERT(scoped_id1->object.initializer == NULL);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations2, "AST node analysis - declarations #2")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_initializer *initializer1 =
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f)));

struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
    &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "declaration_one"), initializer1);
ASSERT(decl1 != NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_qualifier_restrict(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(
    &kft_mem, &decl1->specifiers,
    kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)))));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_auto(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_qualifier_volatile(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

const struct kefir_ast_type *type1 =
    kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                             (struct kefir_ast_type_qualification){.restricted = true, .volatile_type = true});
ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "declaration_one") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 4);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "declaration_one", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
ASSERT(!scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 4);
ASSERT(scoped_id1->object.initializer == initializer1);

ASSERT(scoped_id1->object.initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION);
ASSERT(scoped_id1->object.initializer->expression->klass->type == KEFIR_AST_CONSTANT);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations3, "AST node analysis - declarations #3")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_structure_specifier *specifier1 =
    kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "structure1", true);
struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry1,
    kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field1")),
    NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));
struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                     kefir_ast_type_specifier_double(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry2,
    kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field2")),
    NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
    &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "VaRiAblE123"), NULL);
ASSERT(decl1 != NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(
    &kft_mem, &decl1->specifiers,
    kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 8)))));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_static(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

struct kefir_ast_struct_type *struct_type1 = NULL;
const struct kefir_ast_type *type1 =
    kefir_ast_type_structure(&kft_mem, context->type_bundle, "structure1", &struct_type1);
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "field1",
                                      kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                                                             kefir_ast_type_unsigned_int()),
                                      NULL));
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "field2",
                                      kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_double()),
                                      NULL));
const struct kefir_ast_type *qualifier_type1 = kefir_ast_type_qualified(
    &kft_mem, context->type_bundle, type1, (struct kefir_ast_type_qualification){.constant = true});
ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, qualifier_type1));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "VaRiAblE123") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 8);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "VaRiAblE123", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, qualifier_type1));
ASSERT(!scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 8);
ASSERT(scoped_id1->object.initializer == NULL);

const struct kefir_ast_scoped_identifier *scoped_id2 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_tag_identifier(&local_context, "structure1", &scoped_id2));
ASSERT(scoped_id2->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id2->type, type1));

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations4, "AST node analysis - declarations #4")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
struct kefir_ast_context *context = &global_context.context;

struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
    &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "globalVariable"), NULL);
ASSERT(decl1 != NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_thread_local(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

const struct kefir_ast_type *type1 = kefir_ast_type_unsigned_long_long();
ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "globalVariable") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 0);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "globalVariable", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
ASSERT(!scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 0);
ASSERT(scoped_id1->object.initializer == NULL);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations5, "AST node analysis - declarations #5")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
struct kefir_ast_context *context = &global_context.context;

struct kefir_ast_enum_specifier *specifier1 =
    kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enumeration1", true);
ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols, "XVAL",
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 1000))));
ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols, "YVAL", NULL));
ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols, "AVAL",
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));

struct kefir_ast_declaration *decl1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "counter"), NULL);
ASSERT(decl1 != NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_thread_local(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_enum(&kft_mem, specifier1)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_static(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

struct kefir_ast_enum_type *enum_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
    &kft_mem, context->type_bundle, "enumeration1", context->type_traits->underlying_enumeration_type, &enum_type1);
ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type1, "XVAL",
                                              kefir_ast_constant_expression_integer(&kft_mem, 1000)));
ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "YVAL"));
ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type1, "AVAL",
                                              kefir_ast_constant_expression_integer(&kft_mem, 0)));
ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1));

ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "counter") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 0);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "counter", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
ASSERT(!scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 0);
ASSERT(scoped_id1->object.initializer == NULL);

const struct kefir_ast_scoped_identifier *scoped_id2 = NULL;
ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "XVAL", &scoped_id2));
ASSERT(scoped_id2->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
ASSERT(scoped_id2->enum_constant.value->value.integer == 1000);

const struct kefir_ast_scoped_identifier *scoped_id3 = NULL;
ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "YVAL", &scoped_id3));
ASSERT(scoped_id3->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
ASSERT(scoped_id3->enum_constant.value->value.integer == 1001);

const struct kefir_ast_scoped_identifier *scoped_id4 = NULL;
ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "AVAL", &scoped_id4));
ASSERT(scoped_id4->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT);
ASSERT(scoped_id4->enum_constant.value->value.integer == 0);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations6, "AST node analysis - declarations #6")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
struct kefir_ast_context *context = &global_context.context;

struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &init1->list, NULL,
    kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!")))));

struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                               kefir_ast_declarator_identifier(&kft_mem, context->symbols, "arr")),
    init1);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_char(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

const struct kefir_ast_type *type1 =
    kefir_ast_type_array(&kft_mem, context->type_bundle,
                         kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                  (struct kefir_ast_type_qualification){.constant = true}),
                         kefir_ast_constant_expression_integer(&kft_mem, 14), NULL);

ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "arr") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 0);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "arr", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
ASSERT(!scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 0);
ASSERT(scoped_id1->object.initializer != NULL);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations7, "AST node analysis - declarations #7")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_structure_specifier *specifier1 =
    kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "structureOne", true);
struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
                                                       kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"),
                                                       NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry2,
    kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y")), NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry3,
                                                       kefir_ast_declarator_identifier(&kft_mem, context->symbols, "z"),
                                                       NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));

struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem, NULL, NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &init2->list, NULL,
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
    &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "some_struct"), init2);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(
    &kft_mem, &decl2->specifiers,
    kefir_ast_type_specifier_struct(&kft_mem, kefir_ast_structure_specifier_init(&kft_mem, context->symbols,
                                                                                 "structureOne", false))));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                     kefir_ast_type_qualifier_volatile(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl2)));

struct kefir_ast_struct_type *struct_type1 = NULL;
const struct kefir_ast_type *type1 =
    kefir_ast_type_structure(&kft_mem, context->type_bundle, "structureOne", &struct_type1);
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "x", kefir_ast_type_signed_int(),
                                      NULL));
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "y",
                                      kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
                                      NULL));
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "z", kefir_ast_type_unsigned_long(),
                                      NULL));

const struct kefir_ast_type *type2 = kefir_ast_type_qualified(
    &kft_mem, context->type_bundle, type1, (struct kefir_ast_type_qualification){.volatile_type = true});

ASSERT(decl2->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl2->base.properties.type, type2));
ASSERT(decl2->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl2->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
ASSERT(decl2->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl2->base.properties.declaration_props.identifier, "some_struct") == 0);
ASSERT(decl2->base.properties.declaration_props.alignment == 0);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "some_struct", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type2));
ASSERT(!scoped_id1->object.external);
ASSERT(scoped_id1->object.alignment != NULL);
ASSERT(scoped_id1->object.alignment->value == 0);
ASSERT(scoped_id1->object.initializer == init2);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl2)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations8, "AST node analysis - declarations #8")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_structure_specifier *specifier1 =
    kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "unionS", true);
struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "option1"), NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                     kefir_ast_type_specifier_double(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry2,
    kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "option2")),
    NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_short(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_signed(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry3, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "option3"), NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));

struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                               KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16)),
                               kefir_ast_declarator_identifier(&kft_mem, context->symbols, "unionS_t")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_union(&kft_mem, specifier1)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_storage_class_specifier_typedef(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

struct kefir_ast_struct_type *union_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_union(&kft_mem, context->type_bundle, "unionS", &union_type1);
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type1, "option1", kefir_ast_type_unsigned_int(),
                                      NULL));
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type1, "option2",
                                      kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_double()),
                                      NULL));
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type1, "option3", kefir_ast_type_signed_short(),
                                      NULL));

const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                          kefir_ast_constant_expression_integer(&kft_mem, 16), NULL);

ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type2));
ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF);
ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "unionS_t") == 0);
ASSERT(decl1->base.properties.declaration_props.alignment == 0);

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "unionS_t", &scoped_id1));
ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->type, type2));

struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "Something")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                     kefir_ast_type_specifier_typedef(&kft_mem, context->symbols,
                                                                                      "unionS_t")));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                     kefir_ast_storage_class_specifier_static(&kft_mem)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl2)));

const struct kefir_ast_type *type3 = kefir_ast_type_pointer(&kft_mem, context->type_bundle, type2);

ASSERT(decl2->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
ASSERT(KEFIR_AST_TYPE_SAME(decl2->base.properties.type, type3));
ASSERT(decl2->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(decl2->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
ASSERT(decl2->base.properties.declaration_props.identifier != NULL);
ASSERT(strcmp(decl2->base.properties.declaration_props.identifier, "Something") == 0);
ASSERT(decl2->base.properties.declaration_props.alignment == 0);

const struct kefir_ast_scoped_identifier *scoped_id2 = NULL;
ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "Something", &scoped_id2));
ASSERT(scoped_id2->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
ASSERT(scoped_id2->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
ASSERT(scoped_id2->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id2->object.type, type3));
ASSERT(!scoped_id2->object.external);
ASSERT(scoped_id2->object.alignment != NULL);
ASSERT(scoped_id2->object.alignment->value == 0);
ASSERT(scoped_id2->object.initializer == NULL);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl2)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations9, "AST node analysis - declarations #9")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
struct kefir_ast_context *context = &global_context.context;

struct kefir_ast_declaration *param1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

ASSIGN_DECL_CAST(struct kefir_ast_declaration *, param2,
                 KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(param1))->self);

struct kefir_ast_declaration *decl = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_function(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "func1")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl->specifiers,
                                                     kefir_ast_function_specifier_noreturn(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl->specifiers,
                                                     kefir_ast_storage_class_specifier_static(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl->specifiers,
                                                     kefir_ast_function_specifier_noreturn(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl->specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl->declarator->function.parameters,
                                  kefir_list_tail(&decl->declarator->function.parameters), param1));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl->declarator->function.parameters,
                                  kefir_list_tail(&decl->declarator->function.parameters), param2));

ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl)));

const struct kefir_ast_scoped_identifier *scoped_id = NULL;
ASSERT_OK(context->resolve_ordinary_identifier(context, "func1", &scoped_id));
ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NORETURN);
ASSERT(!scoped_id->function.external);
ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);

struct kefir_ast_function_type *func_type1 = NULL;
const struct kefir_ast_type *type1 =
    kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_float(), "func1", &func_type1);
ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type1, NULL, kefir_ast_type_float(),
                                            &(kefir_ast_scoped_identifier_storage_t){
                                                KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type1, NULL, kefir_ast_type_float(),
                                            &(kefir_ast_scoped_identifier_storage_t){
                                                KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl)));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations10, "AST node analysis - declarations #10")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
struct kefir_ast_context *context = &global_context.context;

struct kefir_ast_structure_specifier *specifier1 =
    kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "param", true);
struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
                                                       kefir_ast_declarator_identifier(&kft_mem, context->symbols,
                                                                                       "field"),
                                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 6))));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
                                                       kefir_ast_declarator_identifier(&kft_mem, context->symbols,
                                                                                       "another_field"),
                                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                     kefir_ast_type_specifier_bool(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "payload"), NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

struct kefir_ast_declaration *param1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

struct kefir_ast_declaration *param2 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "the_param")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
                                                     kefir_ast_type_specifier_struct(&kft_mem, specifier1)));

struct kefir_ast_declarator *decl1 =
    kefir_ast_declarator_function(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "function"));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param1));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param2));

struct kefir_ast_declaration *func1 = kefir_ast_new_declaration(&kft_mem, decl1, NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_storage_class_specifier_extern(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_function_specifier_inline(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_type_specifier_void(&kft_mem)));

ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(func1)));

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(context->resolve_ordinary_identifier(context, "function", &scoped_id1));

struct kefir_ast_struct_type *struct_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "param", &struct_type1);
ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct_type1, "field",
                                         kefir_ast_type_signed_long_long(), NULL,
                                         kefir_ast_constant_expression_integer(&kft_mem, 6)));
ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct_type1, "another_field",
                                         kefir_ast_type_signed_long_long(), NULL,
                                         kefir_ast_constant_expression_integer(&kft_mem, 2)));
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "payload", kefir_ast_type_bool(),
                                      NULL));

struct kefir_ast_function_type *func_type2 = NULL;
const struct kefir_ast_type *type2 =
    kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), "function", &func_type2);
ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type2, NULL, kefir_ast_type_float(),
                                            &(kefir_ast_scoped_identifier_storage_t){
                                                KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type2, "the_param",
                                            kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1),
                                            &(kefir_ast_scoped_identifier_storage_t){
                                                KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

ASSERT(scoped_id1->function.external);
ASSERT(scoped_id1->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
ASSERT(scoped_id1->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->function.type, type2));

const struct kefir_ast_scoped_identifier *scoped_id2 = NULL;
ASSERT(context->resolve_ordinary_identifier(context, "param", &scoped_id2) == KEFIR_NOT_FOUND);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func1)));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations11, "AST node analysis - declarations #11")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_enum_specifier *specifier1 = kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum1", true);
ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols, "A", NULL));
ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols, "B", NULL));
ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols, "C", NULL));

struct kefir_ast_declaration *param1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "p1"), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_specifier_enum(&kft_mem, specifier1)));

struct kefir_ast_declaration *param2 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                               KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "C")),
                               kefir_ast_declarator_identifier(&kft_mem, context->symbols, "p2")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(
    &kft_mem, &param2->specifiers,
    kefir_ast_type_specifier_enum(&kft_mem,
                                  kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum1", false))));

struct kefir_ast_declaration *param3 = kefir_ast_new_declaration(
    &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(
    &kft_mem, &param3->specifiers,
    kefir_ast_type_specifier_union(&kft_mem,
                                   kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "UniOn", false))));

struct kefir_ast_declarator *decl1 =
    kefir_ast_declarator_function(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "fun"));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param1));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param2));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param3));
decl1->function.ellipsis = true;

struct kefir_ast_declaration *func1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_pointer(&kft_mem, decl1), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_function_specifier_inline(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_function_specifier_noreturn(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_type_specifier_char(&kft_mem)));

ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(func1)));

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(context->resolve_ordinary_identifier(context, "fun", &scoped_id1));

struct kefir_ast_enum_type *enum_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
    &kft_mem, context->type_bundle, "enum1", context->type_traits->underlying_enumeration_type, &enum_type1);
ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "A"));
ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "B"));
ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "C"));

struct kefir_ast_function_type *func_type2 = NULL;
const struct kefir_ast_type *type2 = kefir_ast_type_function(
    &kft_mem, context->type_bundle,
    kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                    (struct kefir_ast_type_qualification){.constant = true})),
    "fun", &func_type2);
ASSERT_OK(kefir_ast_type_function_parameter(
    &kft_mem, context->type_bundle, func_type2, "p1",
    kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
                             (struct kefir_ast_type_qualification){.constant = true}),
    &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
ASSERT_OK(kefir_ast_type_function_parameter(
    &kft_mem, context->type_bundle, func_type2, "p2",
    kefir_ast_type_array(&kft_mem, context->type_bundle, type1, kefir_ast_constant_expression_integer(&kft_mem, 2),
                         NULL),
    &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
ASSERT_OK(kefir_ast_type_function_parameter(
    &kft_mem, context->type_bundle, func_type2, NULL,
    kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                           kefir_ast_type_incomplete_union(&kft_mem, context->type_bundle, "UniOn")),
    &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
ASSERT_OK(kefir_ast_type_function_ellipsis(func_type2, true));
ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2));

ASSERT(scoped_id1->function.external);
ASSERT(scoped_id1->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
ASSERT(scoped_id1->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN);
ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->function.type, type2));

const struct kefir_ast_scoped_identifier *scoped_id2 = NULL;
ASSERT(context->resolve_ordinary_identifier(context, "param", &scoped_id2) == KEFIR_NOT_FOUND);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func1)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations12, "AST node analysis - declarations #12")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_declaration *param1 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                               KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)),
                               kefir_ast_declarator_identifier(&kft_mem, context->symbols, "array")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));

ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &param1->declarator->array.type_qualifiers,
                                               KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &param1->declarator->array.type_qualifiers,
                                               KEFIR_AST_TYPE_QUALIFIER_RESTRICT));
param1->declarator->array.static_array = true;

struct kefir_ast_declarator *decl1 = kefir_ast_declarator_function(
    &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "array_processor"));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param1));

struct kefir_ast_declaration *func1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_pointer(&kft_mem, decl1), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_type_specifier_void(&kft_mem)));

ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(func1)));

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(context->resolve_ordinary_identifier(context, "array_processor", &scoped_id1));

struct kefir_ast_function_type *func_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_function(
    &kft_mem, context->type_bundle, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
    "array_processor", &func_type1);
ASSERT_OK(kefir_ast_type_function_parameter(
    &kft_mem, context->type_bundle, func_type1, "array",
    kefir_ast_type_array_static(&kft_mem, context->type_bundle,
                                kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                         (struct kefir_ast_type_qualification){.constant = true}),
                                kefir_ast_constant_expression_integer(&kft_mem, 10),
                                &(struct kefir_ast_type_qualification){.restricted = true, .volatile_type = true}),
    &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->function.type, type1));
ASSERT(scoped_id1->function.external);
ASSERT(scoped_id1->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
ASSERT(scoped_id1->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func1)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations13, "AST node analysis - declarations #13")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_declaration *param1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "length"), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));

struct kefir_ast_declaration *param2 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                               KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "length")),
                               kefir_ast_declarator_identifier(&kft_mem, context->symbols, "array")),
    NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
                                                     kefir_ast_type_qualifier_volatile(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &param2->declarator->array.type_qualifiers,
                                               KEFIR_AST_TYPE_QUALIFIER_CONST));
param2->declarator->array.static_array = true;

struct kefir_ast_declarator *decl1 = kefir_ast_declarator_function(
    &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "array_processor2"));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param1));
ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
                                  param2));

struct kefir_ast_declaration *func1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_pointer(&kft_mem, decl1), NULL);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_function_specifier_inline(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &func1->specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(func1)));

const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
ASSERT_OK(context->resolve_ordinary_identifier(context, "array_processor2", &scoped_id1));

struct kefir_ast_function_type *func_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_function(
    &kft_mem, context->type_bundle, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
    "array_processor2", &func_type1);
ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type1, "length",
                                            kefir_ast_type_signed_long(),
                                            &(kefir_ast_scoped_identifier_storage_t){
                                                KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
ASSERT_OK(kefir_ast_type_function_parameter(
    &kft_mem, context->type_bundle, func_type1, "array",
    kefir_ast_type_vlen_array_static(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                                 (struct kefir_ast_type_qualification){.volatile_type = true}),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "length")),
        &(struct kefir_ast_type_qualification){.constant = true}),
    &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->function.type, type1));
ASSERT(scoped_id1->function.external);
ASSERT(scoped_id1->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
ASSERT(scoped_id1->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func1)));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
