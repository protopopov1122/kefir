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
#include "./expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context,
        "ptr1", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()), NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "ptr2", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_qualified(mem, context->type_bundle,
            kefir_ast_type_char(), (struct kefir_ast_type_qualification){
                .constant = true
            })), NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("equality_arithemtical", {
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'X')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 67)));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1000)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1010)));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.14f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -1)));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.0041f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.2306f)));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 62.475f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.077001)));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 7.001)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3529.001)));
    });

    FUNC("equality_pointer", {
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
        BINARY_NODE(KEFIR_AST_OPERATION_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
    });

    FUNC("non_equality_arithemtical", {
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 34)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\a')));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 100000)));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, -3.14f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 15)));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1e-5)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2e4)));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.781)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1100.0)));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 839.51)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 93871.10001)));
    });

    FUNC("non_equality_pointer", {
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
        BINARY_NODE(KEFIR_AST_OPERATION_NOT_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
    });

    FUNC("less_than", {
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1000)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'Z')));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -10)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'Z')));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 835)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 1)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -844)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 955)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.467f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -400)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 7.33f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.001f)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 10.0007f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1e-6)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.0)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, -0.00078)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 11.45)));
    });

    FUNC("less_equals", {
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1000)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'Z')));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -10)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'Z')));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 835)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 1)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -844)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 955)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.467f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -400)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 7.33f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.001f)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 10.0007f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1e-6)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.0)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, -0.00078)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 11.45)));
    });

    FUNC("greater_than", {
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 671)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'y')));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'P')));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 945)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 0)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1755)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 900)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 295.1f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -553)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 8.21f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 7e-3f)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 14.003f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 4e-3)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.991)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, -0.00875)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 18.002)));
    });

    FUNC("greater_equal", {
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 671)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'y')));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'P')));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 945)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 0)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1755)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 900)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 295.1f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, -553)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 8.21f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 7e-3f)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 14.003f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 4e-3)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.991)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, -0.00875)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 18.002)));
    });

    FUNC("ptr_relational", {
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));

        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
        BINARY_NODE(KEFIR_AST_OPERATION_LESS_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));

        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));

        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
        BINARY_NODE(KEFIR_AST_OPERATION_GREATER_EQUAL,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
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