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

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(mem, context->type_bundle,
        kefir_ast_type_char(), NULL);

    struct kefir_ast_compound_literal *literal1 = kefir_ast_new_compound_literal(mem, type1);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal1->initializer->list,
        NULL, kefir_ast_new_expression_initializer(mem,
            KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "Hello, world!")))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(literal1)));

    REQUIRE_OK(kefir_ast_temporaries_next_block(context->temporaries));

    struct kefir_ast_compound_literal *literal2 = kefir_ast_new_compound_literal(mem, type1);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal2->initializer->list,
        NULL, kefir_ast_new_expression_initializer(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'H')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal2->initializer->list,
        NULL, kefir_ast_new_expression_initializer(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'e')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal2->initializer->list,
        NULL, kefir_ast_new_expression_initializer(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'l')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal2->initializer->list,
        NULL, kefir_ast_new_expression_initializer(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'l')))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal2->initializer->list,
        kefir_ast_new_index_desginator(mem, 1, NULL),
        kefir_ast_new_expression_initializer(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'o')))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(literal2)));

    struct kefir_ast_compound_literal *literal3 = kefir_ast_new_compound_literal(mem, type1);
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(literal3)));

    REQUIRE_OK(kefir_ast_temporaries_next_block(context->temporaries));

    struct kefir_ast_compound_literal *literal4 = kefir_ast_new_compound_literal(mem, type1);
    struct kefir_ast_initializer *init4_1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &literal4->initializer->list, NULL, init4_1));
    struct kefir_ast_initializer *init4_2 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init4_1->list, NULL, init4_2));
    struct kefir_ast_initializer *init4_3 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init4_2->list, NULL, init4_3));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &init4_3->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "test...")))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(literal4)));
            
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

    FUNC("compound_literal2", {
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(literal2);
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("compound_literal3", {
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(literal3);
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("compound_literal4", {
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(literal4);
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
