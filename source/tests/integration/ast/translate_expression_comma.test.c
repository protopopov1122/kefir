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

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_push(mem, &translator_context.type_resolver));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("comma1", {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(comma);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("comma2", {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 21e4))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(comma);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("comma3", {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 21e4))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 66536))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(comma);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("comma4", {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_void(),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true))))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 21e4))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 66536))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(comma);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("comma5", {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'x'))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_void(),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3e-1))))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1 << 4))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(comma);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("comma6", {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'x'))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.00056))));
        REQUIRE_OK(kefir_ast_comma_append(mem, comma,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_void(),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0xfffe))))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(comma);
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