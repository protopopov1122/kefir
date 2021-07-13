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

static struct kefir_ast_function_definition *define_sum_function(struct kefir_mem *mem,
                                                                 const struct kefir_ast_context *context) {

    struct kefir_ast_declarator *function1_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "factorial"));

    REQUIRE(kefir_list_insert_after(
                mem, &function1_decl->function.parameters, kefir_list_tail(&function1_decl->function.parameters),
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i"))) == KEFIR_OK,
            NULL);

    struct kefir_ast_declaration *function1_param1 =
        kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "i"), NULL);
    REQUIRE(kefir_ast_declarator_specifier_list_append(mem, &function1_param1->specifiers,
                                                       kefir_ast_type_specifier_int(mem)) == KEFIR_OK,
            NULL);

    struct kefir_ast_compound_statement *function1_body = kefir_ast_new_compound_statement(mem);

    struct kefir_ast_declaration *stmt1 = kefir_ast_new_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "res"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))));
    REQUIRE(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                    KEFIR_AST_NODE_BASE(stmt1)) == KEFIR_OK,
            NULL);

    struct kefir_ast_node_base *loop_body = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
                 mem, KEFIR_AST_ASSIGNMENT_MULTIPLY,
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "res")),
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i"))))));

    struct kefir_ast_node_base *loop = KEFIR_AST_NODE_BASE(kefir_ast_new_for_statement(
        mem, NULL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_GREATER, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
        KEFIR_AST_NODE_BASE(
            kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i")))),
        loop_body));
    REQUIRE(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                    loop) == KEFIR_OK,
            NULL);

    struct kefir_ast_node_base *stmt2 = KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "res"))));
    REQUIRE(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                    stmt2) == KEFIR_OK,
            NULL);

    struct kefir_ast_function_definition *function1 =
        kefir_ast_new_function_definition(mem, function1_decl, function1_body);
    REQUIRE(kefir_ast_declarator_specifier_list_append(mem, &function1->specifiers,
                                                       kefir_ast_type_specifier_int(mem)) == KEFIR_OK,
            NULL);
    REQUIRE(kefir_list_insert_after(mem, &function1->declarations, kefir_list_tail(&function1->declarations),
                                    function1_param1) == KEFIR_OK,
            NULL);

    return function1;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_ast_function_definition *function = define_sum_function(mem, &global_context.context);
    REQUIRE(function != NULL, KEFIR_INTERNAL_ERROR);

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(function)));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(kefir_ast_translate_function(mem, KEFIR_AST_NODE_BASE(function), &translator_context));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(function)));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
