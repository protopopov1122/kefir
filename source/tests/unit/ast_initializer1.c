/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/initializer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/node.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_initializer_construction1, "AST initializer - construction #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)));
    ASSERT(init1 != NULL);
    ASSERT(init1->type == KEFIR_AST_INITIALIZER_EXPRESSION);
    ASSERT(init1->expression != NULL);
    ASSERT(init1->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) init1->expression->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) init1->expression->self)->value.integer == 100);
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14)));
    ASSERT(init2 != NULL);
    ASSERT(init2->type == KEFIR_AST_INITIALIZER_EXPRESSION);
    ASSERT(init2->expression != NULL);
    ASSERT(init2->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) init2->expression->self)->type == KEFIR_AST_DOUBLE_CONSTANT);
    ASSERT(DOUBLE_EQUALS(((struct kefir_ast_constant *) init2->expression->self)->value.float64, 3.14, DOUBLE_EPSILON));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "test")));
    ASSERT(init3 != NULL);
    ASSERT(init3->type == KEFIR_AST_INITIALIZER_EXPRESSION);
    ASSERT(init3->expression != NULL);
    ASSERT(init3->expression->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(strcmp(((struct kefir_ast_identifier *) init3->expression->self)->identifier, "test") == 0);
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_initializer_construction2, "AST initializer - construction #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT(init1 != NULL);
    ASSERT(kefir_list_length(&init1->list.initializers) == 0);

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
    ASSERT(kefir_list_length(&init1->list.initializers) == 1);

    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "abc")))));
    ASSERT(kefir_list_length(&init1->list.initializers) == 2);

    ASSERT_OK(
        kefir_ast_initializer_list_append(&kft_mem, &init1->list, NULL, kefir_ast_new_list_initializer(&kft_mem)));
    ASSERT(kefir_list_length(&init1->list.initializers) == 3);
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
