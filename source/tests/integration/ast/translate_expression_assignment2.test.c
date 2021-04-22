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

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(mem, context->type_bundle, "", &struct_type1);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type1,
        "field1", kefir_ast_type_bool(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type1,
        "field2", kefir_ast_type_double(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type1,
        "field3", kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_char(),
            kefir_ast_constant_expression_integer(mem, 5), NULL), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type1,
        "field4", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()), NULL));

    struct kefir_ast_struct_type *union_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_union(mem, context->type_bundle, "", &union_type2);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, union_type2,
        "field1", kefir_ast_type_signed_long(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, union_type2,
        "field2", kefir_ast_type_char(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, union_type2,
        "field3", kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_signed_int(),
            kefir_ast_constant_expression_integer(mem, 4), NULL), NULL));

    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context,
        "struct1", type1, NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context,
        "union1", type2, NULL));

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "struct1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_compound_literal(mem, type1))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1));

    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "union1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_compound_literal(mem, type2))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node2));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        &translator_context.type_cache, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        &translator_context.type_cache, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("assign_struct", {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1));
    });

    FUNC("assign_union", {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node2, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node2));
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