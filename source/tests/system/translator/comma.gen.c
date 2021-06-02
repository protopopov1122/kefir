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

#include "codegen.inc.c"

static kefir_result_t define_comma_function(struct kefir_mem *mem,
                                          struct function *func,
                                          struct kefir_ast_context_manager *context_manager,
                                          const char *name,
                                          kefir_size_t count) {
    REQUIRE_OK(kefir_list_init(&func->args));

    const struct kefir_ast_type *payload_type = kefir_ast_type_pointer(mem, context_manager->current->type_bundle,
        kefir_ast_type_void());

    struct kefir_ast_function_type *callback_func_type = NULL;
    const struct kefir_ast_type *callback_type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        kefir_ast_type_signed_int(), NULL, &callback_func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, callback_func_type,
        NULL, payload_type, NULL));
    const struct kefir_ast_type *callback_ptr_type = kefir_ast_type_pointer(mem, context_manager->current->type_bundle,
        callback_type);

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        kefir_ast_type_signed_int(), name, &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type,
        NULL, callback_ptr_type, NULL));
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type,
        NULL, payload_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        func->type, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "callback",
        callback_ptr_type, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "payload",
        payload_type, NULL, NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(mem, &func->args, kefir_list_tail(&func->args), KEFIR_AST_NODE_BASE(
        kefir_ast_new_identifier(mem, context_manager->current->symbols, "callback"))));
    REQUIRE_OK(kefir_list_insert_after(mem, &func->args, kefir_list_tail(&func->args), KEFIR_AST_NODE_BASE(
        kefir_ast_new_identifier(mem, context_manager->current->symbols, "payload"))));

    struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);

    while (count--) {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "callback")));
        REQUIRE_OK(kefir_ast_function_call_append(mem, call,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "payload"))));

        REQUIRE_OK(kefir_ast_comma_append(mem, comma, KEFIR_AST_NODE_BASE(call)));
    }
    func->body = KEFIR_AST_NODE_BASE(comma);
 
    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module, struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function comma1, comma2, comma5,
                    comma10, comma20, comma50;
    REQUIRE_OK(define_comma_function(mem, &comma1, &context_manager, "comma1", 1));
    REQUIRE_OK(define_comma_function(mem, &comma2, &context_manager, "comma2", 2));
    REQUIRE_OK(define_comma_function(mem, &comma5, &context_manager, "comma5", 5));
    REQUIRE_OK(define_comma_function(mem, &comma10, &context_manager, "comma10", 10));
    REQUIRE_OK(define_comma_function(mem, &comma20, &context_manager, "comma20", 20));
    REQUIRE_OK(define_comma_function(mem, &comma50, &context_manager, "comma50", 50));

    REQUIRE_OK(analyze_function(mem, &comma1, &context_manager));
    REQUIRE_OK(analyze_function(mem, &comma2, &context_manager));
    REQUIRE_OK(analyze_function(mem, &comma5, &context_manager));
    REQUIRE_OK(analyze_function(mem, &comma10, &context_manager));
    REQUIRE_OK(analyze_function(mem, &comma20, &context_manager));
    REQUIRE_OK(analyze_function(mem, &comma50, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));
    
    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module,
        &global_context, translator_context.environment, &translator_context.type_cache.resolver,
        &global_scope));

    REQUIRE_OK(translate_function(mem, &comma1, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &comma2, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &comma5, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &comma10, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &comma20, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &comma50, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));
    
    REQUIRE_OK(free_function(mem, &comma1));
    REQUIRE_OK(free_function(mem, &comma2));
    REQUIRE_OK(free_function(mem, &comma5));
    REQUIRE_OK(free_function(mem, &comma10));
    REQUIRE_OK(free_function(mem, &comma20));
    REQUIRE_OK(free_function(mem, &comma50));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
