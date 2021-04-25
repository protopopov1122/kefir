#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/codegen/amd64-sysv.h"

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module, struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_function(mem, context->type_bundle,
        kefir_ast_type_signed_int(), "factorial", &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type,
        NULL, kefir_ast_type_signed_int(), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "x", kefir_ast_type_signed_int(),
        NULL, NULL));

    struct kefir_ast_node_base *node0 = KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "x"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node0));

    struct kefir_ast_function_call *node_call = kefir_ast_new_function_call(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "factorial")));
    REQUIRE_OK(kefir_ast_function_call_append(mem, node_call,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "x")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))))));

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_operator(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "x")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_MULTIPLY,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "x")),
            KEFIR_AST_NODE_BASE(node_call)))
    ));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node0));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context, &env, module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context, &env,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_type_resolver_stack_push(mem, &translator_context.type_resolver));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, module,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &translator_global_scope));

    const struct kefir_ast_translator_resolved_type *resolved_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(mem,
        &translator_context.type_resolver.resolver,
        translator_context.environment,
        context->type_bundle,
        context->type_traits,
        module,
        type,
        &resolved_type));

    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, module,
        resolved_type->function.declaration->ir_function_decl,
        translator_local_scope.local_layout, 0);
    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, &translator_context, &builder, node0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_XCHG, 1));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_STORE32, 0));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context));
    
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node0));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_target_platform ir_target;
    REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&ir_target));
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(generate_ir(mem, &module, &ir_target));

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}