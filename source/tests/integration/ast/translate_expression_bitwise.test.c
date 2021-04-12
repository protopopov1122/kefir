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

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

#define BINARY_NODE(_oper, _node1, _node2) \
    do { \
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem, (_oper), (_node1), (_node2))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node)); \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context)); \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node)); \
    } while (0)

#define FUNC(_id, _init) \
    do { \
        struct kefir_ir_type *func_params = kefir_ir_module_new_type(mem, &module, 0, NULL); \
        struct kefir_ir_type *func_returns = kefir_ir_module_new_type(mem, &module, 0, NULL); \
        struct kefir_ir_function_decl *func_decl = kefir_ir_module_new_function_declaration(mem, &module, \
            (_id), NULL, func_params, false, func_returns); \
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

    FUNC("left_shift", {
        BINARY_NODE(KEFIR_AST_OPERATION_SHIFT_LEFT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'X')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 1)));
        BINARY_NODE(KEFIR_AST_OPERATION_SHIFT_LEFT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1000)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 6)));
        BINARY_NODE(KEFIR_AST_OPERATION_SHIFT_LEFT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 0xcafebabe)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '5')));
    });

    FUNC("right_shift", {
        BINARY_NODE(KEFIR_AST_OPERATION_SHIFT_RIGHT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'T')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 8)));
        BINARY_NODE(KEFIR_AST_OPERATION_SHIFT_RIGHT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 870)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 1)));
        BINARY_NODE(KEFIR_AST_OPERATION_SHIFT_RIGHT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 0xbadbabe)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '0')));
    });

    FUNC("and", {
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '[')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, ']')));
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 87094)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 0xffffeeee)));
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -736402640)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 71)));
    });

    FUNC("or", {
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_OR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\"')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\'')));
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_OR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 666)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 12)));
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_OR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -1001)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1001)));
    });


    FUNC("xor", {
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_XOR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '8')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '0')));
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_XOR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 312)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 304)));
        BINARY_NODE(KEFIR_AST_OPERATION_BITWISE_XOR,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -206)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101)));
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