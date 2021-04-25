#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/scope_manager.h"
#include "kefir/codegen/amd64-sysv.h"

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module, struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_mgr;
    REQUIRE_OK(kefir_ast_context_manager_init(mem, kefir_ast_default_type_traits(), &env.target_env, &context_mgr));
    REQUIRE_OK(kefir_ast_context_manager_open_local(mem, &context_mgr));

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_function(mem, context_mgr.current->type_bundle,
        kefir_ast_type_signed_int(), "factorial", &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_mgr.current->type_bundle, func_type,
        NULL, kefir_ast_type_signed_int(), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_mgr.global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_mgr.local, "x", kefir_ast_type_signed_int(),
        NULL, NULL));

    struct kefir_ast_node_base *x_lvalue = KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_mgr.current->symbols, "x"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_mgr.current, x_lvalue));

    struct kefir_ast_function_call *factorial_call = kefir_ast_new_function_call(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_mgr.current->symbols, "factorial")));
    REQUIRE_OK(kefir_ast_function_call_append(mem, factorial_call,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_mgr.current->symbols, "x")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))))));

    struct kefir_ast_node_base *factorial_node = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_operator(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_mgr.current->symbols, "x")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_MULTIPLY,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_mgr.current->symbols, "x")),
            KEFIR_AST_NODE_BASE(factorial_call)))
    ));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_mgr.current, x_lvalue));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_mgr.current, factorial_node));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context_mgr.current, &env, module));

    struct kefir_ast_translator_scope_manager scope_manager;
    REQUIRE_OK(kefir_ast_translator_scope_manager_init(mem, &translator_context, context_mgr.global,
        &scope_manager));
    REQUIRE_OK(kefir_ast_translator_scope_manager_open_local(mem, &translator_context, context_mgr.local,
        &scope_manager));

    const struct kefir_ast_translator_resolved_type *function_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(mem,
        &translator_context.type_resolver.resolver,
        translator_context.environment,
        context_mgr.current->type_bundle,
        context_mgr.current->type_traits,
        module,
        type,
        &function_type));

    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, module,
        function_type->function.declaration->ir_function_decl,
        scope_manager.local_layout->local_layout, 0);

    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));
    REQUIRE_OK(kefir_ast_translate_lvalue(mem, &translator_context, &builder, x_lvalue));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_XCHG, 1));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_STORE32, 0));
    REQUIRE_OK(kefir_ast_translate_expression(mem, factorial_node, &builder, &translator_context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ast_translator_scope_manager_close_local(mem, &translator_context, &scope_manager));
    REQUIRE_OK(kefir_ast_translator_scope_manager_define_globals(mem, &translator_context, &scope_manager));
    
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, x_lvalue));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, factorial_node));
    REQUIRE_OK(kefir_ast_translator_scope_manager_free(mem, &translator_context, &scope_manager));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_context_manager_free(mem, &context_mgr));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_target_platform ir_target;
    REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&ir_target));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(generate_ir(mem, &module, &ir_target));
    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return EXIT_SUCCESS;
}