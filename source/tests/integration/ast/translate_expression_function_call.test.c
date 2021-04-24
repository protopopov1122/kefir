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
#include "./binary_expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;


    struct kefir_ast_function_type *func_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(mem, context->type_bundle,
        kefir_ast_type_signed_int(), "sum", &func_type1);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type1, NULL, kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type1, NULL, kefir_ast_type_signed_int(), NULL));

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(mem, context->type_bundle,
        kefir_ast_type_signed_int(), "sum2", &func_type2);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type2, NULL, kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_type_function_ellipsis(func_type2, true));

    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1));
    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type2));

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
    struct kefir_irbuilder_block builder;

    FUNC("call1", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum")));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call2", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum")));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.91))));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call3", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum2")));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2))));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3))));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call4", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum2")));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call5", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum2")));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100))));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 4.58f))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
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