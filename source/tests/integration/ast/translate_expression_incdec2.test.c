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

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "int", kefir_ast_type_signed_int(), NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "short", kefir_ast_type_unsigned_short(), NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "float", kefir_ast_type_float(), NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "double", kefir_ast_type_double(), NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "ptr1", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_bool()), NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "ptr2", kefir_ast_type_pointer(mem, context->type_bundle,
            kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_signed_long(),
                kefir_ast_constant_expression_integer(mem, 4), NULL)), NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

#define UNARY_NODE(_oper, _node) \
    do { \
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem, (_oper), (_node))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node)); \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context)); \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node)); \
    } while (0)

#define FUNC(_id, _init) \
    do { \
        struct kefir_ir_type *func_params = kefir_ir_module_new_type(mem, &module, 0, NULL); \
        struct kefir_ir_type *func_returns = kefir_ir_module_new_type(mem, &module, 0, NULL); \
        struct kefir_ir_function_decl *func_decl = kefir_ir_module_new_named_function_declaration(mem, &module, \
            (_id), func_params, false, func_returns); \
        REQUIRE(func_decl != NULL, KEFIR_INTERNAL_ERROR); \
        struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, (_id), translator_local_scope.local_layout, 0); \
        REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body)); \
        _init \
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder)); \
    } while (0)

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        &translator_context.type_cache, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env,
        &translator_context.type_cache, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("increment_int1", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")));
    });

    FUNC("increment_int2", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "short")));
    });

    FUNC("decrement_int1", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")));
    });

    FUNC("decrement_int2", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "short")));
    });

    FUNC("increment_float", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float")));
    });

    FUNC("increment_double", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double")));
    });

    FUNC("decrement_float", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float")));
    });

    FUNC("decrement_double", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double")));
    });

    FUNC("increment_pointer1", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
    });

    FUNC("increment_pointer2", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
    });

    FUNC("decrement_pointer1", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
    });

    FUNC("decrement_pointer2", {
        UNARY_NODE(KEFIR_AST_OPERATION_PREFIX_DECREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
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