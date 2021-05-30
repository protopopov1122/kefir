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

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
        kefir_ast_type_specifier_short(mem)));

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(mem,
        kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name2->type_decl.specifiers,
        kefir_ast_type_specifier_double(mem)));

    struct kefir_ast_type_name *type_name3 = kefir_ast_new_type_name(mem,
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)),
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name3->type_decl.specifiers,
        kefir_ast_type_specifier_char(mem)));

    struct kefir_ast_type_name *type_name4 = kefir_ast_new_type_name(mem,
        kefir_ast_declarator_pointer(mem, 
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name4->type_decl.specifiers,
        kefir_ast_type_specifier_void(mem)));

    FUNC("add_pointer_int", {
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, type_name1,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 67)));
        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, type_name2,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -5)));


        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, type_name3,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101)))));

        BINARY_NODE(KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem, type_name4,
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

        struct kefir_ast_type_name *type_name5 = kefir_ast_new_type_name(mem,
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name5->type_decl.specifiers,
            kefir_ast_type_specifier_char(mem)));

        struct kefir_ast_type_name *type_name6 = kefir_ast_new_type_name(mem,
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name6->type_decl.specifiers,
            kefir_ast_type_specifier_unsigned(mem)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name6->type_decl.specifiers,
            kefir_ast_type_specifier_short(mem)));

        struct kefir_ast_type_name *type_name7 = kefir_ast_new_type_name(mem,
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name7->type_decl.specifiers,
            kefir_ast_type_specifier_int(mem)));

        struct kefir_ast_type_name *type_name8 = kefir_ast_new_type_name(mem,
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name8->type_decl.specifiers,
            kefir_ast_type_specifier_double(mem)));

        struct kefir_ast_type_name *type_name9 = kefir_ast_new_type_name(mem,
            kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name9->type_decl.specifiers,
            kefir_ast_type_specifier_float(mem)));

    FUNC("sub_pointers", {

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name5))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 128)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name5))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 64)))));

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name6))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1024)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name6))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 32)))));

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name7))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 256)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name7))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8)))));

        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name8))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name8))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));
    });

    FUNC("sub_pointer_integer", {
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type_name5,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 16)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type_name6,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 200)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '\t')));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type_name9,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true)));
        BINARY_NODE(KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                type_name8,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 42)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0)));
    });

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(type_name7)));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
