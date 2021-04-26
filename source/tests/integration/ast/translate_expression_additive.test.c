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

    FUNC("add_integral", {
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'x')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'Y')));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -99)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 1001)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 100500)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 200600)));
    });

    FUNC("add_floating_point", {
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.42f)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.763f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\0')));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.141f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.718f)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 100.0001)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 2)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.0001f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.02)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 6492.00054)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 92.315)));
    });

    FUNC("add_pointer_int", {
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_signed_short()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 67)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_double()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -5)));


        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle,
                    kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_char(),
                        kefir_ast_constant_expression_integer(mem, 4), NULL)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101)))));

        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle,
                    kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void())),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)));
    });

    FUNC("sub_integral", {
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'c')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'a')));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 100)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -50)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 123456)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 98765)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\n')),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 1)));
    });

    FUNC("sub_floating_point", {
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 69)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.0045f)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.0f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\\')));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.11f)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.03f)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 2048)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1e5)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.0001)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.00002)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 6.01)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.3f)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.005)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 0.004)));
    });

    FUNC("sub_pointers", {
        const struct kefir_ast_type *type1 = kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_char());
        const struct kefir_ast_type *type2 = kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_unsigned_short());
        const struct kefir_ast_type *type3 = kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_signed_int());
        const struct kefir_ast_type *type4 = kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_double());

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type1,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 128)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type1,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 64)))));

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type2,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1024)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type2,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 32)))));

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type3,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 256)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type3,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8)))));

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type4,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type4,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));
    });

    FUNC("sub_pointer_integer", {
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_char()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 16)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_unsigned_short()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 200)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\t')));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_float()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_double()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 42)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0)));
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