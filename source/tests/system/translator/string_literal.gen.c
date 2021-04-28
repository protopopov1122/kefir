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

static kefir_result_t define_literal_function(struct kefir_mem *mem,
                                            struct function *func,
                                            struct kefir_ast_context_manager *context_manager,
                                            const char *name,
                                            const char *literal) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        kefir_ast_type_pointer(mem, context_manager->current->type_bundle,
            kefir_ast_type_qualified(mem, context_manager->current->type_bundle,
                kefir_ast_type_char(), (struct kefir_ast_type_qualification){
                    .constant = true
                })), name, &func_type);
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

    func->body = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem, KEFIR_AST_OPERATION_ADD,
        KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, literal)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x"))));

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

    struct function literal1, literal2, literal3, literal4;
    REQUIRE_OK(define_literal_function(mem, &literal1, &context_manager, "literal1", "Test123, one two three..."));
    REQUIRE_OK(define_literal_function(mem, &literal2, &context_manager, "literal2", "Hello, world!"));
    REQUIRE_OK(define_literal_function(mem, &literal3, &context_manager, "literal3", ""));
    REQUIRE_OK(define_literal_function(mem, &literal4, &context_manager, "literal4", "\t\t\n\t\tHell \vwrld!\n\t\0"));

    REQUIRE_OK(analyze_function(mem, &literal1, &context_manager));
    REQUIRE_OK(analyze_function(mem, &literal2, &context_manager));
    REQUIRE_OK(analyze_function(mem, &literal3, &context_manager));
    REQUIRE_OK(analyze_function(mem, &literal4, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));
    
    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module,
        &global_context, translator_context.environment, &translator_context.type_cache.resolver,
        &global_scope));

    REQUIRE_OK(translate_function(mem, &literal1, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &literal2, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &literal3, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &literal4, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));
    
    REQUIRE_OK(free_function(mem, &literal1));
    REQUIRE_OK(free_function(mem, &literal2));
    REQUIRE_OK(free_function(mem, &literal3));
    REQUIRE_OK(free_function(mem, &literal4));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}