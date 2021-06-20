#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/flow_control.h"
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
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC2("switch1", {
        struct kefir_ast_declaration *decl1 =
            kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "request"), NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_long(mem)));

        struct kefir_ast_declaration *decl2 =
            kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "response"), NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_long(mem)));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -2))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -3))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, NULL,
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));

        struct kefir_ast_compound_statement *compound2 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                           KEFIR_AST_NODE_BASE(decl1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                           KEFIR_AST_NODE_BASE(decl2)));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_switch_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "request")),
                KEFIR_AST_NODE_BASE(compound1)))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound2);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("switch2", {
        struct kefir_ast_declaration *decl1 =
            kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "request"), NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_long(mem)));

        struct kefir_ast_declaration *decl2 =
            kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "response"), NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_long(mem)));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));

        struct kefir_ast_compound_statement *compound2 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                             mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "response")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10))))))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                mem, NULL,
                KEFIR_AST_NODE_BASE(kefir_ast_new_switch_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "request")),
                    KEFIR_AST_NODE_BASE(compound1)))))));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));

        struct kefir_ast_compound_statement *compound3 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound3->block_items, kefir_list_tail(&compound3->block_items),
                                           KEFIR_AST_NODE_BASE(decl1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound3->block_items, kefir_list_tail(&compound3->block_items),
                                           KEFIR_AST_NODE_BASE(decl2)));
        REQUIRE_OK(kefir_list_insert_after(
            mem, &compound3->block_items, kefir_list_tail(&compound3->block_items),
            KEFIR_AST_NODE_BASE(kefir_ast_new_switch_statement(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "request")),
                KEFIR_AST_NODE_BASE(compound2)))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound3);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
