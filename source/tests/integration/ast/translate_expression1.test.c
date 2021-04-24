#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/ir/builder.h"
#include "kefir/ast/node.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ir/format.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1",
            decl_params, false, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, NULL, 0);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, decl_result, KEFIR_IR_TYPE_INT, 0, 0));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(),
        &kft_util_get_translator_environment()->target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context,
        &local_context.context, kft_util_get_translator_environment(), &module));

    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));
    struct kefir_ast_binary_operation *ast = kefir_ast_new_binary_operation(mem,
        KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_MULTIPLY,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem,
                KEFIR_AST_OPERATION_INVERT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 2)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 3)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem,
            KEFIR_AST_OPERATION_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 1)))));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &local_context.context, KEFIR_AST_NODE_BASE(ast)));
    REQUIRE_OK(kefir_ast_translate_expression(mem, KEFIR_AST_NODE_BASE(ast), &builder, &translator_context));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(ast)));

    ast = kefir_ast_new_binary_operation(mem,
        KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_MULTIPLY,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
                KEFIR_AST_OPERATION_SHIFT_LEFT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 2)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.0)));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &local_context.context, KEFIR_AST_NODE_BASE(ast)));
    REQUIRE_OK(kefir_ast_translate_expression(mem, KEFIR_AST_NODE_BASE(ast), &builder, &translator_context));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(ast)));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    REQUIRE_OK(kefir_ir_format_module(stdout, &module));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}