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

#define muldiv(_id, _oper) \
    FUNC((_id), { \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -150))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 156)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 239))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -76)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1000))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -1)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 4.56f))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.098f)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A'))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 73.9f)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, -7366.440f))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1024)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1000.004))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.000056)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -7))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.01)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.05f))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 90.4f)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 10.01))); \
        BINARY_NODE((_oper), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.00005)), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem,  200000.01))); \
    })

    muldiv("multiply", KEFIR_AST_OPERATION_MULTIPLY);
    muldiv("divide", KEFIR_AST_OPERATION_DIVIDE);

    FUNC("modulo", {
        BINARY_NODE(KEFIR_AST_OPERATION_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'b')));
        BINARY_NODE(KEFIR_AST_OPERATION_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1005)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '%')));
        BINARY_NODE(KEFIR_AST_OPERATION_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -255)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 1024)));
        BINARY_NODE(KEFIR_AST_OPERATION_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 10001)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)));
        BINARY_NODE(KEFIR_AST_OPERATION_MODULO,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 6000)));
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