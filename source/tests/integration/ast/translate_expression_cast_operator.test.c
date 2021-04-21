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

    #define CAST_NODE(_type, _node) \
        do { \
            struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, \
                (_type), (_node))); \
            REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1)); \
            REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context)); \
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1)); \
        } while (0)

    struct kefir_ast_struct_type *struct1_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(mem, context->type_bundle,
        "", &struct1_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct1_type,
        "field_one", kefir_ast_type_char(), NULL));

    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context,
        "ptr", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()), NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context,
        "structure", type1, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

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

    struct kefir_ir_type *func1_params = kefir_ir_module_new_type(mem, &module, 0, NULL);
    struct kefir_ir_type *func1_returns = kefir_ir_module_new_type(mem, &module, 0, NULL);

    struct kefir_ir_function_decl *func1_decl = kefir_ir_module_new_named_function_declaration(mem, &module,
        "func1", func1_params, false, func1_returns, NULL);
    REQUIRE(func1_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 = kefir_ir_module_new_function(mem, &module, "func1", translator_local_scope.local_layout, 0);
    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func1->body));

    CAST_NODE(kefir_ast_type_bool(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -150)));
    CAST_NODE(kefir_ast_type_signed_int(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)));
    CAST_NODE(kefir_ast_type_signed_int(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true)));
    CAST_NODE(kefir_ast_type_signed_short(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 100)));
    CAST_NODE(kefir_ast_type_signed_long(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.14f)));
    CAST_NODE(kefir_ast_type_signed_long(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3.14)));
    CAST_NODE(kefir_ast_type_signed_long_long(), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));

    CAST_NODE(kefir_ast_type_unsigned_int(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1234)));
    CAST_NODE(kefir_ast_type_unsigned_short(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101)));
    CAST_NODE(kefir_ast_type_unsigned_long(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.71f)));
    CAST_NODE(kefir_ast_type_unsigned_long(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 2.71)));
    CAST_NODE(kefir_ast_type_unsigned_long_long(), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));

    CAST_NODE(kefir_ast_type_float(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -67)));
    CAST_NODE(kefir_ast_type_float(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 67)));
    CAST_NODE(kefir_ast_type_float(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.069f)));
    CAST_NODE(kefir_ast_type_float(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 42.0)));

    CAST_NODE(kefir_ast_type_double(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100500)));
    CAST_NODE(kefir_ast_type_double(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1597)));
    CAST_NODE(kefir_ast_type_double(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.0112f)));
    CAST_NODE(kefir_ast_type_double(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 42.1)));

    CAST_NODE(kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_signed_short()),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -0xfee)));
    CAST_NODE(kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_bool()),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0xcade)));
    CAST_NODE(kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_float()),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));

    CAST_NODE(kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10000)));
    CAST_NODE(kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 10001)));
    CAST_NODE(kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 16.7f)));
    CAST_NODE(kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 982.0001)));
    CAST_NODE(kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr")));
    CAST_NODE(kefir_ast_type_void(), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "structure")));

#undef CAST_NODE
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}