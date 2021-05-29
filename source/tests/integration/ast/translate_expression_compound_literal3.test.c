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
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry1,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)),
            kefir_ast_declarator_identifier(mem, context->symbols, "field1")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry2,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "field2")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry3,
        kefir_ast_declarator_identifier(mem, context->symbols, "field3"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry3));

    struct kefir_ast_structure_specifier *specifier2 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry4->declaration.specifiers,
        kefir_ast_type_specifier_struct(mem, kefir_ast_structure_specifier_clone(mem, specifier1))));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry4,
        kefir_ast_declarator_identifier(mem, context->symbols, "X"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2, entry4));

    struct kefir_ast_structure_declaration_entry *entry5 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry5->declaration.specifiers,
        kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry5,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)),
            kefir_ast_declarator_identifier(mem, context->symbols, "Y")),
        NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2, entry5));

    struct kefir_ast_structure_specifier *specifier3 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry6 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry6->declaration.specifiers,
        kefir_ast_type_specifier_union(mem, specifier2)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry6,
        kefir_ast_declarator_identifier(mem, context->symbols, "hello"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier3, entry6));

    struct kefir_ast_structure_declaration_entry *entry7 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry7->declaration.specifiers,
        kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry7,
        kefir_ast_declarator_identifier(mem, context->symbols, "cruel"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier3, entry7));

    struct kefir_ast_structure_declaration_entry *entry8 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry8->declaration.specifiers,
        kefir_ast_type_specifier_struct(mem, specifier1)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry8,
        kefir_ast_declarator_identifier(mem, context->symbols, "world"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier3, entry8));

    struct kefir_ast_type_name *type_name3 = kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name3->type_decl.specifiers,
        kefir_ast_type_specifier_struct(mem, specifier3)));

    struct kefir_ast_compound_literal *literal1 = kefir_ast_new_compound_literal(mem, type_name3);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL, kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "WTF?")))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        kefir_ast_new_member_desginator(mem, context->symbols, "field3",
            kefir_ast_new_member_desginator(mem, context->symbols, "X",
                kefir_ast_new_member_desginator(mem, context->symbols, "hello", NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        kefir_ast_new_index_desginator(mem, 1,
            kefir_ast_new_member_desginator(mem, context->symbols, "Y",
                kefir_ast_new_member_desginator(mem, context->symbols, "hello", NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)))));

    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list,
        kefir_ast_new_index_desginator(mem, 8, NULL),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'B')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init1->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'B')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        kefir_ast_new_member_desginator(mem, context->symbols, "field1",
            kefir_ast_new_member_desginator(mem, context->symbols, "world", NULL)),
        init1));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 7)))));

    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        kefir_ast_new_member_desginator(mem, context->symbols, "field3",
            kefir_ast_new_member_desginator(mem, context->symbols, "X",
                kefir_ast_new_member_desginator(mem, context->symbols, "hello", NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 9)))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(literal1)));
            
    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("compound_literal1", {
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(literal1);
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
