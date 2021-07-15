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

#include "codegen.inc.c"

static kefir_result_t define_conditional_function(struct kefir_mem *mem, struct function *func,
                                                  struct kefir_ast_context_manager *context_manager) {
    REQUIRE_OK(kefir_list_init(&func->args));

    const struct kefir_ast_type *arg_type = kefir_ast_type_unbounded_array(
        mem, context_manager->current->type_bundle,
        kefir_ast_type_qualified(mem, context_manager->current->type_bundle, kefir_ast_type_char(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL);
    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, kefir_ast_type_signed_long_long(),
                                         "parseint", &func_type);
    REQUIRE_OK(
        kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL, arg_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->type, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(kefir_ast_local_context_define_auto(
        mem, context_manager->local, "str",
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context_manager->current->type_bundle, arg_type), NULL, NULL, NULL));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "str"))));

    struct kefir_ast_compound_statement *compound0 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declaration *declarationValue = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "value"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationValue->specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationValue->specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(declarationValue)));

    struct kefir_ast_declaration *declarationSign = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "sign"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationSign->specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(declarationSign)));

    struct kefir_ast_node_base *condition1 = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_NOT_EQUAL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "str")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));

    struct kefir_ast_node_base *condition2 = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_NOT_EQUAL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_INDIRECTION,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "str")))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));

    struct kefir_ast_node_base *condition = KEFIR_AST_NODE_BASE(
        kefir_ast_new_binary_operation(mem, KEFIR_AST_OPERATION_LOGICAL_AND, condition1, condition2));

    struct kefir_ast_compound_statement *body = kefir_ast_new_compound_statement(mem);

    struct kefir_ast_declaration *declarationChar = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "chr"),
        kefir_ast_new_expression_initializer(
            mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                mem, KEFIR_AST_OPERATION_INDIRECTION,
                KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                    mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "str"))))))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationChar->specifiers,
                                                          kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &body->block_items, kefir_list_tail(&body->block_items),
                                       KEFIR_AST_NODE_BASE(declarationChar)));

    struct kefir_ast_compound_statement *loop_cond1_body = kefir_ast_new_compound_statement(mem);
    REQUIRE_OK(kefir_list_insert_after(
        mem, &loop_cond1_body->block_items, kefir_list_tail(&loop_cond1_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                     mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sign")),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1))))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &loop_cond1_body->block_items,
                                       kefir_list_tail(&loop_cond1_body->block_items),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_continue_statement(mem))));

    struct kefir_ast_node_base *loop_cond1 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
        mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_BITWISE_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                mem, KEFIR_AST_OPERATION_EQUAL,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "chr")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '-')))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                mem, KEFIR_AST_OPERATION_EQUAL,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sign")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 0)))))),
        KEFIR_AST_NODE_BASE(loop_cond1_body),
        KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
            mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                mem, KEFIR_AST_OPERATION_EQUAL,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sign")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                mem,
                KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sign")),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))))),
            NULL))));
    REQUIRE_OK(kefir_list_insert_after(mem, &body->block_items, kefir_list_tail(&body->block_items), loop_cond1));

    struct kefir_ast_node_base *loop_cond2_condition = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_LOGICAL_AND,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "chr")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '0')))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "chr")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '9'))))));

    struct kefir_ast_node_base *loop_cond2_then = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                 mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value")),
                 KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                     mem, KEFIR_AST_OPERATION_ADD,
                     KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                         mem, KEFIR_AST_OPERATION_MULTIPLY,
                         KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value")),
                         KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)))),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                         mem, KEFIR_AST_OPERATION_SUBTRACT,
                         KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "chr")),
                         KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '0'))))))))));

    struct kefir_ast_node_base *loop_cond2 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
        mem, loop_cond2_condition, loop_cond2_then, KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));
    REQUIRE_OK(kefir_list_insert_after(mem, &body->block_items, kefir_list_tail(&body->block_items), loop_cond2));

    struct kefir_ast_while_statement *loop = kefir_ast_new_while_statement(mem, condition, KEFIR_AST_NODE_BASE(body));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(loop)));

    struct kefir_ast_return_statement *returnStatement = kefir_ast_new_return_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                 mem, KEFIR_AST_OPERATION_MULTIPLY,
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value")),
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sign")))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(returnStatement)));

    func->body = KEFIR_AST_NODE_BASE(compound0);

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

    struct function func;
    REQUIRE_OK(define_conditional_function(mem, &func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &func, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(translate_function(mem, &func, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &func));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
