#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

static kefir_result_t define_variables(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                       struct kefir_list *variables) {
    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "character1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'B'))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "integer1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0x4efd))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_declaration *decl3 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "long1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -100000))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl3)));

    struct kefir_ast_declaration *decl4 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "float1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 7.6549f))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl4->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl4->specifiers, kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl4)));

    struct kefir_ast_declaration *decl5 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "double1"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.547e-9))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl5->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl5->specifiers, kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl5)));

    struct kefir_ast_declaration *decl6 = kefir_ast_new_declaration(
        mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(mem, context->symbols, "str1")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "Test...test...test...."))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl6->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl6->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl6)));

    struct kefir_ast_declaration *decl7 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "str2")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "TEXT-ONE-TWO-THREE"))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl7->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl7->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl7)));

    struct kefir_ast_declaration *decl8 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "str3")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "str1"))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl8->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl8->specifiers, kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl8)));

    struct kefir_ast_declaration *decl9 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "int1ptr")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                     mem, KEFIR_AST_OPERATION_ADDRESS,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "integer1"))))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl9->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl9->specifiers, kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl9)));

    struct kefir_ast_declaration *decl10 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "fn1")), NULL);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl10->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl10)));

    struct kefir_ast_declaration *decl11 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "fn1ptr")),
        kefir_ast_new_expression_initializer(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "fn1"))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl11->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl11->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl11)));

    struct kefir_ast_declaration *decl12 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, context->symbols, "null_ptr")),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl12->specifiers,
                                                          kefir_ast_storage_class_specifier_extern(mem)));
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &decl12->specifiers, kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, variables, kefir_list_tail(variables), KEFIR_AST_NODE_BASE(decl12)));

    return KEFIR_OK;
}

static kefir_result_t free_variable(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                    void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(entry != NULL, KEFIR_INTERNAL_ERROR);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_list variables;
    REQUIRE_OK(kefir_list_init(&variables));
    REQUIRE_OK(kefir_list_on_remove(&variables, free_variable, NULL));

    REQUIRE_OK(define_variables(mem, &global_context.context, &variables));

    for (const struct kefir_list_entry *iter = kefir_list_head(&variables); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, node));
    }

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(kefir_list_free(mem, &variables));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
