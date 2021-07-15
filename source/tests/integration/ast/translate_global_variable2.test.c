#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/test/util.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/format.h"
#include "./expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "variable", kefir_ast_type_signed_int(),
                                                         NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&global_translator_context, &global_context.context, &env, &module));
    global_translator_context.global_scope_layout = &translator_global_scope;

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "integer"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "floatingPoint"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 13.14f))));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));

    struct kefir_ast_declaration *decl3 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "floatingPoint2"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.001))));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));

    struct kefir_ast_declaration *decl4 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "character"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'a'))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl4->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl4->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));

    struct kefir_ast_declaration *decl5 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "ptr1")),
        kefir_ast_new_expression_initializer(
            mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                mem, KEFIR_AST_OPERATION_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                    mem, KEFIR_AST_OPERATION_ADDRESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, global_context.context.symbols, "integer")))),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl5->specifiers, kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl5->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                               kefir_ast_type_specifier_short(mem));

    struct kefir_ast_declaration *decl6 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "ptr2")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                     mem, KEFIR_AST_OPERATION_ADD,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                         mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10))))));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl6->specifiers, kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl6->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl2)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl3)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl4)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl5)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(decl6)));

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl2)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl3)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl4)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl5)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl6)));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
