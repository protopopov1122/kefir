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

    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context,
        "int", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context,
        "ullong", kefir_ast_type_unsigned_long_long(), NULL));

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

#define COMPOUND_ASSIGNMENT(_oper, _node1, _node2) \
    do { \
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(mem, (_oper), \
            (_node1), (_node2))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node)); \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context)); \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node)); \
    } while (0)

    FUNC("assign_modulo_int", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)));
    });

    FUNC("assign_modulo_ullong", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -8000)));
    });

    FUNC("assign_shl_int", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_SHIFT_LEFT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)));
    });

    FUNC("assign_shl_ullong", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_SHIFT_LEFT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 11223344ll)));
    });

    FUNC("assign_shr_int", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 90)));
    });

    FUNC("assign_shr_ullong", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -99000)));
    });

    FUNC("assign_bitwise_and_int", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_BITWISE_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10101)));
    });

    FUNC("assign_biwise_and_ullong", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_BITWISE_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -11001100)));
    });

    FUNC("assign_bitwise_or_int", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_BITWISE_OR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'h')));
    });

    FUNC("assign_biwise_or_ullong", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_BITWISE_OR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 1)));
    });

    FUNC("assign_bitwise_xor_int", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_BITWISE_XOR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 19857)));
    });

    FUNC("assign_biwise_xor_ullong", {
        COMPOUND_ASSIGNMENT(KEFIR_AST_ASSIGNMENT_BITWISE_XOR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ullong")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '8')));
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