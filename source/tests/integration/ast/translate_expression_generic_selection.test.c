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
#include <stdio.h>

static struct kefir_ast_node_base *make_gen_selection(struct kefir_mem *mem,
                                                    const struct kefir_ast_context *context,
                                                    struct kefir_ast_node_base *arg) {
    struct kefir_ast_generic_selection *generic_selection1 = kefir_ast_new_generic_selection(mem,
        arg);
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        kefir_ast_type_char(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)));
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        kefir_ast_type_signed_short(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)));
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        kefir_ast_type_unsigned_int(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)));
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        kefir_ast_type_float(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)));
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        kefir_ast_type_double(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)));
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6)));
    kefir_ast_generic_selection_append(mem, generic_selection1, context->type_traits,
        NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 7)));
    return KEFIR_AST_NODE_BASE(generic_selection1);
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_node_base *node1 = make_gen_selection(mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.14)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1));

    struct kefir_ast_node_base *node2 = make_gen_selection(mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, kefir_ast_type_char(),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node2));

    struct kefir_ast_node_base *node3 = make_gen_selection(mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, kefir_ast_type_signed_short(),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 54)))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node3));

    struct kefir_ast_node_base *node4 = make_gen_selection(mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 3)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node4));

    struct kefir_ast_node_base *node5 = make_gen_selection(mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 2.71)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node5));

    struct kefir_ast_node_base *node6 = make_gen_selection(mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'B')));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node6));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));

    struct kefir_ir_type *func1_params = kefir_ir_module_new_type(mem, &module, 0, NULL);
    struct kefir_ir_type *func1_returns = kefir_ir_module_new_type(mem, &module, 0, NULL);

    struct kefir_ir_function_decl *func1_decl = kefir_ir_module_new_function_declaration(mem, &module,
        "func1", func1_params, false, func1_returns);
    REQUIRE(func1_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 = kefir_ir_module_new_function(mem, &module, func1_decl,
        translator_local_scope.local_layout, 0);
    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func1->body));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node2, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node3, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node4, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node5, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node6, &builder, &translator_context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node2));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node3));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node4));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node5));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node6));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}