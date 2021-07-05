#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_node_analysis_function_definitions1, "AST node analysis - function definitions #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));

    struct kefir_ast_declarator_specifier_list specifiers;
    REQUIRE_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(
        kefir_ast_declarator_specifier_list_append(&kft_mem, &specifiers, kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(
        &kft_mem, kefir_ast_declarator_function(
                      &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "fn1")));

    struct kefir_ast_declaration *param1 = kefir_ast_new_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "param1"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                         kefir_ast_storage_class_specifier_register(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declarator->pointer.declarator->function.parameters,
                                      kefir_list_tail(&declarator->pointer.declarator->function.parameters), param1));

    struct kefir_ast_declaration *param2 = kefir_ast_new_declaration(
        &kft_mem,
        kefir_ast_declarator_pointer(
            &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "param2")),
        NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declarator->pointer.declarator->function.parameters,
                                      kefir_list_tail(&declarator->pointer.declarator->function.parameters), param2));

    struct kefir_ast_node_base *stmt1 = KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
        &kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, global_context.context.symbols, "param1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, global_context.context.symbols, "param2"))))));

    struct kefir_ast_compound_statement *body = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &body->block_items, kefir_list_tail(&body->block_items), stmt1));

    struct kefir_ast_function_definition *func = kefir_ast_new_function_definition(&kft_mem, declarator, body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_move_all(&func->specifiers, &specifiers));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &global_context.context, KEFIR_AST_NODE_BASE(func)));

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_function(
        &kft_mem, global_context.context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, global_context.context.type_bundle, kefir_ast_type_signed_int()), "fn1",
        &func_type);
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, global_context.context.type_bundle, func_type, "param1", kefir_ast_type_unsigned_int(),
        &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER}));
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, global_context.context.type_bundle, func_type, "param2",
        kefir_ast_type_pointer(&kft_mem, global_context.context.type_bundle, kefir_ast_type_signed_int()),
        &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

    ASSERT(func->base.properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION);
    ASSERT(func->base.properties.function_definition.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(strcmp(func->base.properties.function_definition.identifier, "fn1") == 0);
    ASSERT(func->base.properties.function_definition.scoped_id != NULL);
    ASSERT(func->base.properties.function_definition.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(!func->base.properties.function_definition.scoped_id->function.external);
    ASSERT(func->base.properties.function_definition.scoped_id->function.local_context != NULL);
    ASSERT(func->base.properties.function_definition.scoped_id->function.specifier ==
           KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(func->base.properties.function_definition.scoped_id->function.storage ==
           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_SAME(func->base.properties.function_definition.scoped_id->function.type, type));
    ASSERT(func->base.properties.function_definition.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);

    ASSERT(param1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(strcmp(param1->base.properties.declaration_props.identifier, "param1") == 0);
    ASSERT(!param1->base.properties.declaration_props.static_assertion);
    ASSERT(param1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(param1->base.properties.declaration_props.alignment == 0);
    ASSERT(param1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    ASSERT(param1->base.properties.declaration_props.scoped_id != NULL);
    ASSERT(param1->base.properties.declaration_props.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(KEFIR_AST_TYPE_SAME(param1->base.properties.declaration_props.scoped_id->object.type,
                               kefir_ast_type_unsigned_int()));

    ASSERT(param2->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(strcmp(param2->base.properties.declaration_props.identifier, "param2") == 0);
    ASSERT(!param2->base.properties.declaration_props.static_assertion);
    ASSERT(param2->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(param2->base.properties.declaration_props.alignment == 0);
    ASSERT(param2->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN);
    ASSERT(param2->base.properties.declaration_props.scoped_id != NULL);
    ASSERT(param2->base.properties.declaration_props.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(KEFIR_AST_TYPE_SAME(
        param2->base.properties.declaration_props.scoped_id->object.type,
        kefir_ast_type_pointer(&kft_mem, global_context.context.type_bundle, kefir_ast_type_signed_int())));

    ASSERT(body->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(body->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(body->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT(stmt1->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->properties.statement_props.flow_control_statement == NULL);

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(global_context.context.resolve_ordinary_identifier(&global_context.context, "fn1", &scoped_id));
    ASSERT(func->base.properties.function_definition.scoped_id == scoped_id);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_function_definitions2, "AST node analysis - function definitions #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));

    struct kefir_ast_declarator_specifier_list specifiers;
    REQUIRE_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &specifiers,
                                                         kefir_ast_storage_class_specifier_static(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &specifiers,
                                                         kefir_ast_function_specifier_inline(&kft_mem)));
    ASSERT_OK(
        kefir_ast_declarator_specifier_list_append(&kft_mem, &specifiers, kefir_ast_type_specifier_double(&kft_mem)));

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_function(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "pi"));

    struct kefir_ast_node_base *stmt1 = KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14159))));

    struct kefir_ast_compound_statement *body = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &body->block_items, kefir_list_tail(&body->block_items), stmt1));

    struct kefir_ast_function_definition *func = kefir_ast_new_function_definition(&kft_mem, declarator, body);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_move_all(&func->specifiers, &specifiers));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &global_context.context, KEFIR_AST_NODE_BASE(func)));

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_function(&kft_mem, global_context.context.type_bundle,
                                                                kefir_ast_type_double(), "pi", &func_type);

    ASSERT(func->base.properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION);
    ASSERT(func->base.properties.function_definition.function == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
    ASSERT(strcmp(func->base.properties.function_definition.identifier, "pi") == 0);
    ASSERT(func->base.properties.function_definition.scoped_id != NULL);
    ASSERT(func->base.properties.function_definition.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(!func->base.properties.function_definition.scoped_id->function.external);
    ASSERT(func->base.properties.function_definition.scoped_id->function.local_context != NULL);
    ASSERT(func->base.properties.function_definition.scoped_id->function.specifier ==
           KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
    ASSERT(func->base.properties.function_definition.scoped_id->function.storage ==
           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(KEFIR_AST_TYPE_SAME(func->base.properties.function_definition.scoped_id->function.type, type));
    ASSERT(func->base.properties.function_definition.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);

    ASSERT(body->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(body->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(body->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT(stmt1->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->properties.statement_props.flow_control_statement == NULL);

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(global_context.context.resolve_ordinary_identifier(&global_context.context, "pi", &scoped_id));
    ASSERT(func->base.properties.function_definition.scoped_id == scoped_id);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_function_definitions3, "AST node analysis - function definitions #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));

    struct kefir_ast_declarator_specifier_list specifiers;
    REQUIRE_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(
        kefir_ast_declarator_specifier_list_append(&kft_mem, &specifiers, kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(
        &kft_mem, kefir_ast_declarator_function(
                      &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "fn1")));

    struct kefir_list declarations;
    REQUIRE_OK(kefir_list_init(&declarations));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "param1"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_storage_class_specifier_register(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &declarations, kefir_list_tail(&declarations), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
        &kft_mem,
        kefir_ast_declarator_pointer(
            &kft_mem, kefir_ast_declarator_identifier(&kft_mem, global_context.context.symbols, "param2")),
        NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &declarations, kefir_list_tail(&declarations), KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_node_base *param1 =
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, global_context.context.symbols, "param1"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declarator->pointer.declarator->function.parameters,
                                      kefir_list_tail(&declarator->pointer.declarator->function.parameters), param1));

    struct kefir_ast_node_base *param2 =
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, global_context.context.symbols, "param2"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &declarator->pointer.declarator->function.parameters,
                                      kefir_list_tail(&declarator->pointer.declarator->function.parameters), param2));

    struct kefir_ast_node_base *stmt1 = KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
        &kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, global_context.context.symbols, "param1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, global_context.context.symbols, "param2"))))));

    struct kefir_ast_compound_statement *body = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &body->block_items, kefir_list_tail(&body->block_items), stmt1));

    struct kefir_ast_function_definition *func = kefir_ast_new_function_definition(&kft_mem, declarator, body);
    REQUIRE_OK(kefir_list_move_all(&func->declarations, &declarations));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_move_all(&func->specifiers, &specifiers));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &global_context.context, KEFIR_AST_NODE_BASE(func)));

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_function(
        &kft_mem, global_context.context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, global_context.context.type_bundle, kefir_ast_type_signed_int()), "fn1",
        &func_type);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, global_context.context.type_bundle, func_type, "param1", NULL,
                                                NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, global_context.context.type_bundle, func_type, "param2", NULL,
                                                NULL));

    ASSERT(func->base.properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION);
    ASSERT(func->base.properties.function_definition.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(strcmp(func->base.properties.function_definition.identifier, "fn1") == 0);
    ASSERT(func->base.properties.function_definition.scoped_id != NULL);
    ASSERT(func->base.properties.function_definition.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(!func->base.properties.function_definition.scoped_id->function.external);
    ASSERT(func->base.properties.function_definition.scoped_id->function.local_context != NULL);
    ASSERT(func->base.properties.function_definition.scoped_id->function.specifier ==
           KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(func->base.properties.function_definition.scoped_id->function.storage ==
           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_SAME(func->base.properties.function_definition.scoped_id->function.type, type));
    ASSERT(func->base.properties.function_definition.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);

    // ASSERT(param1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    // ASSERT(strcmp(param1->base.properties.declaration_props.identifier, "param1") == 0);
    // ASSERT(!param1->base.properties.declaration_props.static_assertion);
    // ASSERT(param1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    // ASSERT(param1->base.properties.declaration_props.alignment == 0);
    // ASSERT(param1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    // ASSERT(param1->base.properties.declaration_props.scoped_id != NULL);
    // ASSERT(param1->base.properties.declaration_props.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    // ASSERT(KEFIR_AST_TYPE_SAME(param1->base.properties.declaration_props.scoped_id->object.type,
    //                            kefir_ast_type_unsigned_int()));

    // ASSERT(param2->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    // ASSERT(strcmp(param2->base.properties.declaration_props.identifier, "param2") == 0);
    // ASSERT(!param2->base.properties.declaration_props.static_assertion);
    // ASSERT(param2->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    // ASSERT(param2->base.properties.declaration_props.alignment == 0);
    // ASSERT(param2->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN);
    // ASSERT(param2->base.properties.declaration_props.scoped_id != NULL);
    // ASSERT(param2->base.properties.declaration_props.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    // ASSERT(KEFIR_AST_TYPE_SAME(
    //     param2->base.properties.declaration_props.scoped_id->object.type,
    //     kefir_ast_type_pointer(&kft_mem, global_context.context.type_bundle, kefir_ast_type_signed_int())));

    ASSERT(body->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(body->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(body->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT(stmt1->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->properties.statement_props.flow_control_statement == NULL);

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(global_context.context.resolve_ordinary_identifier(&global_context.context, "fn1", &scoped_id));
    ASSERT(func->base.properties.function_definition.scoped_id == scoped_id);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(func)));
    ASSERT_OK(kefir_list_free(&kft_mem, &declarations));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
