#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

#include "codegen.inc.c"

static kefir_result_t define_sum_function(struct kefir_mem *mem, struct function *func,
                                          struct kefir_ast_context_manager *context_manager, const char *name,
                                          struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, NULL));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers, specifier));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, KEFIR_AST_NODE_BASE(type_name1)));

    const struct kefir_ast_type *param_type =
        kefir_ast_type_pointer(mem, context_manager->current->type_bundle, type_name1->base.properties.type);

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, kefir_ast_type_signed_int(), name,
                                         &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL,
                                                 param_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->type, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "param", param_type, NULL, NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "param"))));

    struct kefir_ast_node_base *add4 = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            mem, context_manager->current->symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "param")), "d")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            mem, context_manager->current->symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "param")), "e"))));

    struct kefir_ast_node_base *add3 = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            mem, context_manager->current->symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "param")), "c")),
        add4));

    struct kefir_ast_node_base *add2 = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            mem, context_manager->current->symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "param")), "b")),
        add3));

    struct kefir_ast_node_base *add1 = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            mem, context_manager->current->symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "param")), "a")),
        add2));

    struct kefir_ast_node_base *adds = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_SIZEOF,
                                                          KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name1)))),
        add1));

    struct kefir_ast_node_base *adda = KEFIR_AST_NODE_BASE(
        kefir_ast_new_binary_operation(mem, KEFIR_AST_OPERATION_ADD,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                                           mem, KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(type_name1))),
                                       adds));

    func->body = adda;

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(mem, context_manager.current->symbols, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_signed(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "a"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_signed(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry2, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "b"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5))));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
                                                          kefir_ast_type_specifier_signed(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
                                                          kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry3, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "c"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3))));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry3));

    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry4->declaration.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry4, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "d"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 17))));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry4));

    struct kefir_ast_structure_declaration_entry *entry5 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry5->declaration.specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry5, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "e"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 30))));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry5));

    struct function sum;
    REQUIRE_OK(
        define_sum_function(mem, &sum, &context_manager, "sum", kefir_ast_type_specifier_struct(mem, specifier1)));

    REQUIRE_OK(analyze_function(mem, &sum, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));

    REQUIRE_OK(translate_function(mem, &sum, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &sum));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
