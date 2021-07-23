/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

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
#include "kefir/ast/alignment.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct kefir_ast_alignment *kefir_ast_alignment_default(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_alignment *alignment = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_alignment));
    REQUIRE(alignment != NULL, NULL);
    *alignment = (const struct kefir_ast_alignment){0};
    alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
    alignment->value = KEFIR_AST_DEFAULT_ALIGNMENT;
    return alignment;
}

struct kefir_ast_alignment *kefir_ast_alignment_as_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    struct kefir_ast_alignment *alignment = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_alignment));
    REQUIRE(alignment != NULL, NULL);
    *alignment = (const struct kefir_ast_alignment){0};
    alignment->klass = KEFIR_AST_ALIGNMENT_AS_TYPE;
    alignment->value = KEFIR_AST_DEFAULT_ALIGNMENT;
    alignment->type = type;
    return alignment;
}

struct kefir_ast_alignment *kefir_ast_alignment_const_expression(struct kefir_mem *mem,
                                                                 struct kefir_ast_constant_expression *const_expr) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(const_expr != NULL, NULL);
    REQUIRE(const_expr->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE ||
                const_expr->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
            NULL);
    struct kefir_ast_alignment *alignment = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_alignment));
    REQUIRE(alignment != NULL, NULL);
    *alignment = (const struct kefir_ast_alignment){0};
    alignment->klass = KEFIR_AST_ALIGNMENT_AS_CONST_EXPR;
    alignment->const_expr = const_expr;

    if (const_expr->value.klass != KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE) {
        REQUIRE_ELSE(const_expr->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER, {
            KEFIR_FREE(mem, alignment);
            return NULL;
        });
        alignment->value = const_expr->value.integer;
    }
    return alignment;
}

kefir_result_t kefir_ast_alignment_free(struct kefir_mem *mem, struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));
    switch (alignment->klass) {
        case KEFIR_AST_ALIGNMENT_DEFAULT:
            break;

        case KEFIR_AST_ALIGNMENT_AS_TYPE:
            alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
            alignment->type = NULL;
            break;

        case KEFIR_AST_ALIGNMENT_AS_CONST_EXPR:
            alignment->klass = KEFIR_AST_ALIGNMENT_DEFAULT;
            REQUIRE_OK(kefir_ast_constant_expression_free(mem, alignment->const_expr));
            alignment->const_expr = NULL;
            break;
    }
    KEFIR_FREE(mem, alignment);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_alignment_evaluate(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                            struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(alignment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST alignment"));

    switch (alignment->klass) {
        case KEFIR_AST_ALIGNMENT_DEFAULT:
            alignment->value = KEFIR_AST_DEFAULT_ALIGNMENT;
            break;

        case KEFIR_AST_ALIGNMENT_AS_TYPE: {
            kefir_ast_target_environment_opaque_type_t target_type;
            struct kefir_ast_target_environment_object_info type_info;
            REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env, alignment->type, &target_type));
            kefir_result_t res =
                KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env, target_type, NULL, &type_info);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, target_type);
                return res;
            });
            alignment->value = type_info.alignment;
            REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, target_type));
        } break;

        case KEFIR_AST_ALIGNMENT_AS_CONST_EXPR: {
            REQUIRE_OK(kefir_ast_constant_expression_evaluate(mem, context, alignment->const_expr));
            REQUIRE(alignment->const_expr->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected integral constant expression as an alignment"));
            alignment->value = alignment->const_expr->value.integer;
        } break;
    }
    return KEFIR_OK;
}
