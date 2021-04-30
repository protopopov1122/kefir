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

static kefir_result_t define_cast_function(struct kefir_mem *mem,
                                         struct function *func,
                                         struct kefir_ast_context_manager *context_manager,
                                         const char *name,
                                         const struct kefir_ast_type *param_type,
                                         const struct kefir_ast_type *return_type) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        return_type, name, &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type,
        NULL, param_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        func->type));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "value",
        param_type, NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(mem, &func->args, kefir_list_tail(&func->args), KEFIR_AST_NODE_BASE(
        kefir_ast_new_identifier(mem, context_manager->current->symbols, "value"))));

    func->body = KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, return_type,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value"))
    ));
 
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

    struct function char_int_cast, int_char_cast,
                    uchar_int_cast, int_uchar_cast,
                    char_uint_cast, uint_char_cast,
                    uchar_uint_cast, uint_uchar_cast,
                    long_float_cast, float_long_cast;
    REQUIRE_OK(define_cast_function(mem, &char_int_cast, &context_manager, "char_int_cast",
        kefir_ast_type_char(), kefir_ast_type_signed_int()));
    REQUIRE_OK(define_cast_function(mem, &int_char_cast, &context_manager, "int_char_cast",
        kefir_ast_type_signed_int(), kefir_ast_type_char()));
    REQUIRE_OK(define_cast_function(mem, &uchar_int_cast, &context_manager, "uchar_int_cast",
        kefir_ast_type_unsigned_char(), kefir_ast_type_signed_int()));
    REQUIRE_OK(define_cast_function(mem, &int_uchar_cast, &context_manager, "int_uchar_cast",
        kefir_ast_type_signed_int(), kefir_ast_type_unsigned_char()));
    REQUIRE_OK(define_cast_function(mem, &char_uint_cast, &context_manager, "char_uint_cast",
        kefir_ast_type_char(), kefir_ast_type_unsigned_int()));
    REQUIRE_OK(define_cast_function(mem, &uint_char_cast, &context_manager, "uint_char_cast",
        kefir_ast_type_signed_int(), kefir_ast_type_unsigned_char()));
    REQUIRE_OK(define_cast_function(mem, &uchar_uint_cast, &context_manager, "uchar_uint_cast",
        kefir_ast_type_unsigned_char(), kefir_ast_type_unsigned_int()));
    REQUIRE_OK(define_cast_function(mem, &uint_uchar_cast, &context_manager, "uint_uchar_cast",
        kefir_ast_type_unsigned_int(), kefir_ast_type_unsigned_char()));
    REQUIRE_OK(define_cast_function(mem, &long_float_cast, &context_manager, "long_float_cast",
        kefir_ast_type_signed_long(), kefir_ast_type_float()));
    REQUIRE_OK(define_cast_function(mem, &float_long_cast, &context_manager, "float_long_cast",
        kefir_ast_type_float(), kefir_ast_type_signed_long_long()));

    REQUIRE_OK(analyze_function(mem, &char_int_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &int_char_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uchar_int_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &int_uchar_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &char_uint_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uint_char_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uchar_uint_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uint_uchar_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &long_float_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &float_long_cast, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));
    
    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module,
        &global_context, translator_context.environment, &translator_context.type_cache.resolver,
        &global_scope));

    REQUIRE_OK(translate_function(mem, &char_int_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &int_char_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uchar_int_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &int_uchar_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &char_uint_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uint_char_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uchar_uint_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uint_uchar_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &long_float_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &float_long_cast, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));
    
    REQUIRE_OK(free_function(mem, &char_int_cast));
    REQUIRE_OK(free_function(mem, &int_char_cast));
    REQUIRE_OK(free_function(mem, &uchar_int_cast));
    REQUIRE_OK(free_function(mem, &int_uchar_cast));
    REQUIRE_OK(free_function(mem, &char_uint_cast));
    REQUIRE_OK(free_function(mem, &uint_char_cast));
    REQUIRE_OK(free_function(mem, &uchar_uint_cast));
    REQUIRE_OK(free_function(mem, &uint_uchar_cast));
    REQUIRE_OK(free_function(mem, &long_float_cast));
    REQUIRE_OK(free_function(mem, &float_long_cast));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}