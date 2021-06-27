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

    struct kefir_ast_function_type *func_type = NULL;

    const struct kefir_ast_type *arg_type =
        kefir_ast_type_unbounded_array(mem, context_manager->current->type_bundle, kefir_ast_type_signed_long(), NULL);

    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, kefir_ast_type_unsigned_long(),
                                         "count_positive", &func_type);
    REQUIRE_OK(
        kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL, arg_type, NULL));
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL,
                                                 kefir_ast_type_unsigned_long(), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->type, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(kefir_ast_local_context_define_auto(
        mem, context_manager->local, "sequence",
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context_manager->current->type_bundle, arg_type), NULL, NULL, NULL));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sequence"))));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "maxlen",
                                                   kefir_ast_type_unsigned_long(), NULL, NULL, NULL));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "maxlen"))));

    struct kefir_ast_compound_statement *compound0 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declaration *declarationCount = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "count"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationCount->specifiers,
                                                          kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationCount->specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(declarationCount)));

    struct kefir_ast_comma_operator *loop_condition = kefir_ast_new_comma_operator(mem);
    REQUIRE_OK(kefir_ast_comma_append(
        mem, loop_condition,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sequence"))))));
    REQUIRE_OK(kefir_ast_comma_append(
        mem, loop_condition,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "maxlen"))))));

    struct kefir_ast_compound_statement *loop_body = kefir_ast_new_compound_statement(mem);

    struct kefir_ast_node_base *loop_body_stmt1 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
        mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                mem, KEFIR_AST_OPERATION_INDIRECTION,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sequence")))))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_continue_statement(mem)), NULL));
    REQUIRE_OK(kefir_list_insert_after(mem, &loop_body->block_items, kefir_list_tail(&loop_body->block_items),
                                       loop_body_stmt1));

    struct kefir_ast_node_base *loop_body_stmt2 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
        mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                mem, KEFIR_AST_OPERATION_INDIRECTION,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sequence")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem)), NULL));
    REQUIRE_OK(kefir_list_insert_after(mem, &loop_body->block_items, kefir_list_tail(&loop_body->block_items),
                                       loop_body_stmt2));

    struct kefir_ast_node_base *loop_body_stmt3 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                 mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "count"))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &loop_body->block_items, kefir_list_tail(&loop_body->block_items),
                                       loop_body_stmt3));

    struct kefir_ast_node_base *loop = KEFIR_AST_NODE_BASE(
        kefir_ast_new_do_while_statement(mem, KEFIR_AST_NODE_BASE(loop_condition), KEFIR_AST_NODE_BASE(loop_body)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items), loop));

    struct kefir_ast_return_statement *returnStatement = kefir_ast_new_return_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "count")));
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

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));

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
