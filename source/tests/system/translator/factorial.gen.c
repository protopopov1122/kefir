#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

struct function {
    const struct kefir_ast_type *type;
    struct kefir_ast_local_context local_context;
    struct kefir_list args;
    struct kefir_ast_node_base *body;
};

static kefir_result_t define_factorial_function(struct kefir_mem *mem,
                                    struct function *func,
                                    struct kefir_ast_context_manager *context_manager) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        kefir_ast_type_signed_int(), "factorial", &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type,
        NULL, kefir_ast_type_signed_int(), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        func->type));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "x", kefir_ast_type_signed_int(),
        NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(mem, &func->args, kefir_list_tail(&func->args), KEFIR_AST_NODE_BASE(
        kefir_ast_new_identifier(mem, context_manager->current->symbols, "x"))));

    struct kefir_ast_function_call *factorial_call = kefir_ast_new_function_call(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "factorial")));
    REQUIRE_OK(kefir_ast_function_call_append(mem, factorial_call,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))))));

    func->body = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_operator(mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem,
            KEFIR_AST_OPERATION_MULTIPLY,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x")),
            KEFIR_AST_NODE_BASE(factorial_call)))
    ));
 
    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t analyze_function(struct kefir_mem *mem,
                                     struct function *func,
                                     struct kefir_ast_context_manager *context_manager) {
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));
    for (const struct kefir_list_entry *iter = kefir_list_head(&func->args);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg,
            iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, arg));
    }
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, func->body));
    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t translate_function(struct kefir_mem *mem,
                                       struct function *func,
                                       struct kefir_ast_context_manager *context_manager,
                                       struct kefir_ast_translator_global_scope_layout *global_scope,
                                       struct kefir_ast_translator_context *translator_context) {
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));
    
    const struct kefir_ast_translator_resolved_type *function_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(mem,
        &translator_context->type_cache.resolver,
        translator_context->environment,
        context_manager->current->type_bundle,
        context_manager->current->type_traits,
        translator_context->module,
        func->type,
        &function_type));

    struct kefir_ast_translator_context local_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init_local(&local_translator_context, &func->local_context.context, translator_context));

    struct kefir_ast_translator_local_scope_layout local_scope;
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, translator_context->module, global_scope, &local_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &func->local_context,
        local_translator_context.environment, local_translator_context.module, &local_translator_context.type_cache.resolver,
        &local_scope));

    struct kefir_ir_function *ir_func = kefir_ir_module_new_function(mem, local_translator_context.module,
        function_type->function.declaration->ir_function_decl,
        local_scope.local_layout, 0);

    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &ir_func->body));
    for (const struct kefir_list_entry *iter = kefir_list_tail(&func->args);
        iter != NULL;
        iter = iter->prev) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg,
            iter->value);
        REQUIRE_OK(kefir_ast_translate_lvalue(mem, &local_translator_context, &builder, arg));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_XCHG, 1));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_STORE32, 0));
    REQUIRE_OK(kefir_ast_translate_expression(mem, func->body, &builder, &local_translator_context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &local_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &local_translator_context));
    return KEFIR_OK;
}

static kefir_result_t free_function(struct kefir_mem *mem,
                                  struct function *func) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&func->args);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg,
            iter->value);
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, arg));
    }
    REQUIRE_OK(kefir_list_free(mem, &func->args));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, func->body));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &func->local_context));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module, struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function func;
    REQUIRE_OK(define_factorial_function(mem, &func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &func, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));
    
    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module,
        &global_context, translator_context.environment, &translator_context.type_cache.resolver,
        &global_scope));
    REQUIRE_OK(translate_function(mem, &func, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));
    
    REQUIRE_OK(free_function(mem, &func));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}