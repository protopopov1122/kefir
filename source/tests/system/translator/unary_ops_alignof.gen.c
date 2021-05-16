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

static kefir_result_t define_get_alignof_function(struct kefir_mem *mem,
                                           struct function *func,
                                           struct kefir_ast_context_manager *context_manager,
                                           const struct kefir_ast_type *struct_type) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        struct_type, "get_alignof", &func_type);

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        func->type));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    struct kefir_ast_compound_literal *compound_literal = kefir_ast_new_compound_literal(mem, struct_type);
#define APPEND(_type) \
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &compound_literal->initializer->list, \
        NULL, kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE( \
            kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_ALIGNOF, \
                KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(mem, (_type)))) \
        ))))
    APPEND(kefir_ast_type_bool());
    APPEND(kefir_ast_type_char());
    APPEND(kefir_ast_type_unsigned_char());
    APPEND(kefir_ast_type_signed_char());
    APPEND(kefir_ast_type_unsigned_short());
    APPEND(kefir_ast_type_signed_short());
    APPEND(kefir_ast_type_unsigned_int());
    APPEND(kefir_ast_type_signed_int());
    APPEND(kefir_ast_type_unsigned_long());
    APPEND(kefir_ast_type_signed_long());
    APPEND(kefir_ast_type_unsigned_long_long());
    APPEND(kefir_ast_type_signed_long_long());
    APPEND(kefir_ast_type_float());
    APPEND(kefir_ast_type_double());
    APPEND(kefir_ast_type_pointer(mem, context_manager->current->type_bundle, kefir_ast_type_void()));
    APPEND(struct_type);
    APPEND(kefir_ast_type_array(mem, context_manager->current->type_bundle,
        kefir_ast_type_signed_int(), kefir_ast_constant_expression_integer(mem, 128), NULL));

    func->body = KEFIR_AST_NODE_BASE(compound_literal);

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

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_structure(mem, context_manager.current->type_bundle,
        "", &struct_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tboolean", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tchar", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tuchar", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tschar", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tushort", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tsshort", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tuint", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tsint", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tulong", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tslong", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tullong", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tsllong", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tfloat", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tdouble", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tpvoid", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tstruct", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type,
        "tarray", kefir_ast_type_signed_int(), NULL));

    struct function get_alignof;
    REQUIRE_OK(define_get_alignof_function(mem, &get_alignof, &context_manager, type));

    REQUIRE_OK(analyze_function(mem, &get_alignof, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));
    
    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module,
        &global_context, translator_context.environment, &translator_context.type_cache.resolver,
        &global_scope));

    REQUIRE_OK(translate_function(mem, &get_alignof, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));
    
    REQUIRE_OK(free_function(mem, &get_alignof));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
