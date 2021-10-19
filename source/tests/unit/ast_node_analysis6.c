/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

#define ASSERT_FUNCTION_CALL(_mem, _context, _id, _type, _args)                                         \
    do {                                                                                                \
        struct kefir_ast_function_call *call1 = kefir_ast_new_function_call(                            \
            (_mem), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        _args ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(call1)));        \
        ASSERT(call1->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);                  \
        ASSERT(KEFIR_AST_TYPE_SAME(call1->base.properties.type, (_type)));                              \
        ASSERT(!call1->base.properties.expression_props.constant_expression);                           \
        ASSERT(!call1->base.properties.expression_props.lvalue);                                        \
        ASSERT(!call1->base.properties.expression_props.addressable);                                   \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(call1));                                        \
    } while (0)

#define ASSERT_FUNCTION_CALL_NOK(_mem, _context, _id, _args)                                            \
    do {                                                                                                \
        struct kefir_ast_function_call *call1 = kefir_ast_new_function_call(                            \
            (_mem), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        _args ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(call1)));       \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(call1));                                        \
    } while (0)

DEFINE_CASE(ast_node_analysis_function_calls1, "AST node analysis - function calls #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_float(), &function1);

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &function2);

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                       "func1", type1, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
                                                       "func2", type2, NULL, NULL));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {});
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_void(), {});
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_void(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_void(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_function_calls2, "AST node analysis - function calls #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(), &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, kefir_ast_type_signed_long(),
                                                NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, kefir_ast_type_double(), NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, kefir_ast_type_char(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(), &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2, kefir_ast_type_signed_long(),
                                                NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2, kefir_ast_type_double(), NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2, kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_type_function_ellipsis(function2, true));

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                       "func1", type1, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                       "func2", type2, NULL, NULL));

    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {});
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0f))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f))));
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });

    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func2", {});
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func2", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_unsigned_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_unsigned_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0f))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_unsigned_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_function_calls3, "AST node analysis - function calls #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(), &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                       "func1", type1, NULL, NULL));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {});
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0f))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_function_calls4, "AST node analysis - function calls #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_float(), &function1);
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1, kefir_ast_type_void(), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                       "func1", type1, NULL, NULL));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {});
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

#define ASSERT_COMPOUND_LITERAL(_mem, _context, _type, _init, _result_type, _constant)                              \
    do {                                                                                                            \
        struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal(                               \
            (_mem), (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE((_mem), KEFIR_AST_NODE_BASE(_type))->self); \
        ASSERT(compound != NULL);                                                                                   \
        _init ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(compound)));                 \
        ASSERT(compound->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);                           \
        ASSERT(KEFIR_AST_TYPE_SAME(compound->base.properties.type, (_result_type)));                                \
        ASSERT(compound->base.properties.expression_props.constant_expression == (_constant));                      \
        ASSERT(compound->base.properties.expression_props.lvalue);                                                  \
        ASSERT(compound->base.properties.expression_props.addressable);                                             \
        ASSERT(!compound->base.properties.expression_props.bitfield);                                               \
        ASSERT(!compound->base.properties.expression_props.string_literal.content);                                 \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(compound)));                                      \
    } while (0)

#define ASSERT_COMPOUND_LITERAL_NOK(_mem, _context, _type, _init)                                                   \
    do {                                                                                                            \
        struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal(                               \
            (_mem), (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE((_mem), KEFIR_AST_NODE_BASE(_type))->self); \
        ASSERT(compound != NULL);                                                                                   \
        _init ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(compound)));                \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(compound)));                                      \
    } while (0)

DEFINE_CASE(ast_node_analysis_compound_literal1, "AST node analysis - compound literals #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "x", kefir_ast_type_signed_int(), NULL,
                                                       NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "y",
                                                      kefir_ast_constant_expression_integer(&kft_mem, 100),
                                                      type_traits->underlying_enumeration_type, NULL, NULL));

    struct kefir_ast_type_name *TYPES[] = {
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)),
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    const struct kefir_ast_type *TYPES2[] = {
        kefir_ast_type_boolean(),          kefir_ast_type_char(),         kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),      kefir_ast_type_unsigned_int(), kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),    kefir_ast_type_signed_long(),  kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(), kefir_ast_type_float(),        kefir_ast_type_double()};
    _Static_assert(sizeof(TYPES2) / sizeof(TYPES2[0]) == sizeof(TYPES) / sizeof(TYPES[0]),
                   "Type array length mismatch");

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[0]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_bool(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[1]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[7]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[9]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[9]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[10]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[11]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, TYPES[i], {});

        ASSERT_COMPOUND_LITERAL(
            &kft_mem, context, TYPES[i],
            {
                ASSERT_OK(kefir_ast_initializer_list_append(
                    &kft_mem, &compound->initializer->list, NULL,
                    kefir_ast_new_expression_initializer(
                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)))));
            },
            TYPES2[i], true);

        ASSERT_COMPOUND_LITERAL(
            &kft_mem, context, TYPES[i],
            {
                ASSERT_OK(kefir_ast_initializer_list_append(
                    &kft_mem, &compound->initializer->list, NULL,
                    kefir_ast_new_expression_initializer(
                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))));
            },
            TYPES2[i], false);

        ASSERT_COMPOUND_LITERAL(
            &kft_mem, context, TYPES[i],
            {
                ASSERT_OK(kefir_ast_initializer_list_append(
                    &kft_mem, &compound->initializer->list, NULL,
                    kefir_ast_new_expression_initializer(
                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")))));
            },
            TYPES2[i], true);

        if (i != 0) {
            ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, TYPES[i], {
                ASSERT_OK(kefir_ast_initializer_list_append(
                    &kft_mem, &compound->initializer->list, NULL,
                    kefir_ast_new_expression_initializer(
                        &kft_mem,
                        KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
            });
        } else {
            ASSERT_COMPOUND_LITERAL(
                &kft_mem, context, TYPES[i],
                {
                    ASSERT_OK(kefir_ast_initializer_list_append(
                        &kft_mem, &compound->initializer->list, NULL,
                        kefir_ast_new_expression_initializer(
                            &kft_mem,
                            KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
                },
                TYPES2[i], true);
        }
    }

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(TYPES[i])));
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_compound_literal2, "AST node analysis - compound literals #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(
        &kft_mem, &local_context, "x",
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 10), NULL),
        NULL, NULL, NULL));

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type_name1, {},
                            kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                 kefir_ast_constant_expression_integer(&kft_mem, 0), NULL),
                            true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem,
                    KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
        },
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 14), NULL),
        true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'o')))));
        },
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 5), NULL),
        true);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type_name2, {});

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_char(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem,
                    KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")))));
        },
        type2, true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'o')))));
        },
        type2, true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))));
        },
        type2, true);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_compound_literal3, "AST node analysis - compound literals #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(&kft_mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field1"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field2"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry3,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field3")),
        NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));

    struct kefir_ast_structure_specifier *specifier2 = kefir_ast_structure_specifier_init(&kft_mem, NULL, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry4->declaration.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry4, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier2, entry4));

    struct kefir_ast_structure_declaration_entry *entry5 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry5->declaration.specifiers,
                                                         kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry5,
        kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                   KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
                                   kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y")),
        NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier2, entry5));

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(
        &kft_mem, &type_name1->type_decl.specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, kefir_ast_structure_specifier_clone(&kft_mem, specifier2))));

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_struct(&kft_mem, specifier2)));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(type_name2)));

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "field1", kefir_ast_type_signed_int(),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "field2", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1, "field3",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
                                          NULL));

    struct kefir_ast_struct_type *structure2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &structure2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2, "x", kefir_ast_type_float(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2, "y",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
                                                         kefir_ast_constant_expression_integer(&kft_mem, 2), NULL),
                                    NULL));

    const struct kefir_ast_type *type3 = kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, type2, NULL);

    ASSERT(KEFIR_AST_TYPE_SAME(type2, type_name1->base.properties.type));
    ASSERT(KEFIR_AST_TYPE_SAME(type3, type_name2->base.properties.type));

    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &local_context, "var1", type1, NULL, NULL,
                                                                    NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var2", type2, NULL, NULL, NULL));

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type_name1, {}, type2, true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        },
        type2, true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        },
        type2, true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        },
        type2, false);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        },
        type2, false);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type_name1, {
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    });

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        },
        type2, false);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                                                                   &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                                                                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(
                                                                       &kft_mem, context->symbols, "var1")))))));
        },
        type2, false);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        },
        type2, true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type_name2, {},
                            kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
                                                 kefir_ast_constant_expression_integer(&kft_mem, 0), NULL),
                            true);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type_name2, {
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
    });

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        },
        kefir_ast_type_array(&kft_mem, context->type_bundle, type2, kefir_ast_constant_expression_integer(&kft_mem, 1),
                             NULL),
        false);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        },
        kefir_ast_type_array(&kft_mem, context->type_bundle, type2, kefir_ast_constant_expression_integer(&kft_mem, 4),
                             NULL),
        false);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type_name2, {
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    });

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        },
        kefir_ast_type_array(&kft_mem, context->type_bundle, type2, kefir_ast_constant_expression_integer(&kft_mem, 2),
                             NULL),
        true);

    ASSERT_COMPOUND_LITERAL(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        },
        kefir_ast_type_array(&kft_mem, context->type_bundle, type2, kefir_ast_constant_expression_integer(&kft_mem, 2),
                             NULL),
        false);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type_name2, {
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(&kft_mem,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(
            &kft_mem, &compound->initializer->list, NULL,
            kefir_ast_new_expression_initializer(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    });

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

#undef ASSERT_COMPOUND_LITERAL
#undef ASSERT_COMPOUND_LITERAL_NOK

#define ASSERT_COMPOUND_LITERAL_TEMP(_mem, _context, _type_name, _init, _id, _member)                       \
    do {                                                                                                    \
        struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal((_mem), (_type_name)); \
        ASSERT(compound != NULL);                                                                           \
        _init ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(compound)));         \
        ASSERT(compound->base.properties.expression_props.temporary.identifier == (_id));                   \
        ASSERT(compound->base.properties.expression_props.temporary.field == (_member));                    \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(compound)));                              \
    } while (0)

DEFINE_CASE(ast_node_analysis_compound_literal4, "AST node analysis - compound literals temporaries") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT(context->resolve_ordinary_identifier(context, KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id) ==
           KEFIR_NOT_FOUND);

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_type_name *type_name2 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));

    struct kefir_ast_type_name *type_name3 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));

    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name1,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));
        },
        0, 0);
    ASSERT_OK(context->resolve_ordinary_identifier(context, KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id));

    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name2,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        },
        0, 1);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name3,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));
        },
        0, 2);

    ASSERT_OK(kefir_ast_temporaries_next_block(context->temporaries));

    struct kefir_ast_type_name *type_name4 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_type_name *type_name5 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name5->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name5->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));

    struct kefir_ast_type_name *type_name6 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name6->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));

    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name4,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));
        },
        1, 0);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name5,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        },
        1, 1);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name6,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));
        },
        1, 2);

    ASSERT_OK(kefir_ast_temporaries_next_block(context->temporaries));

    struct kefir_ast_type_name *type_name7 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name7->type_decl.specifiers,
                                                         kefir_ast_type_specifier_bool(&kft_mem)));

    struct kefir_ast_type_name *type_name8 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name8->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));

    struct kefir_ast_type_name *type_name9 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name9->type_decl.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_type_name *type_name10 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name10->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name10->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));

    struct kefir_ast_type_name *type_name11 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name11->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));

    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context, type_name7,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)))));
        },
        2, 0);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, context,
        (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name8)),
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10000)))));
        },
        2, 1);

    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, &global_context.context, type_name8,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10000)))));
        },
        0, 0);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, &global_context.context, type_name9,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));
        },
        0, 1);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, &global_context.context, type_name10,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(&kft_mem,
                                                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        },
        0, 2);
    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, &global_context.context,
        (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name11)),
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));
        },
        0, 3);
    ASSERT_OK(kefir_ast_temporaries_next_block(&global_context.temporaries));

    ASSERT_COMPOUND_LITERAL_TEMP(
        &kft_mem, &global_context.context, type_name11,
        {
            ASSERT_OK(kefir_ast_initializer_list_append(
                &kft_mem, &compound->initializer->list, NULL,
                kefir_ast_new_expression_initializer(
                    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));
        },
        1, 0);

    ASSERT_OK(context->resolve_ordinary_identifier(context, KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
