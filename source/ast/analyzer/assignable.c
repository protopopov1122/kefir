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

#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_type_assignable(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_type *value_type, kefir_bool_t constant_expression,
                                         const struct kefir_ast_type *target_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(value_type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid value AST type"));
    REQUIRE(target_type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid target AST type"));

    if (target_type->tag == KEFIR_AST_TYPE_SCALAR_BOOL && value_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        // Intentionally left empty
    } else if (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(target_type)) {
        REQUIRE(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(value_type),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Assignable node shall have arithmetic type"));
    } else if (target_type->tag == KEFIR_AST_TYPE_STRUCTURE || target_type->tag == KEFIR_AST_TYPE_UNION) {
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, target_type, value_type),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Both assignable operands shall be compatible"));
    } else if (target_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER && value_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        struct kefir_ast_type_qualification target_qualification, value_qualification;
        const struct kefir_ast_type *target_underlying = kefir_ast_unqualified_type(target_type->referenced_type);
        const struct kefir_ast_type *value_underlying = kefir_ast_unqualified_type(value_type->referenced_type);
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&target_qualification, target_type->referenced_type));
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&value_qualification, value_type->referenced_type));

        if (target_underlying->tag != KEFIR_AST_TYPE_VOID && value_underlying->tag != KEFIR_AST_TYPE_VOID) {
            REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, target_underlying, value_underlying),
                    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Both assignable operands shall be pointers to compatible types"));
        }
        REQUIRE((!value_qualification.constant || target_qualification.constant) &&
                    (!value_qualification.restricted || target_qualification.restricted) &&
                    (!value_qualification.volatile_type || target_qualification.volatile_type),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH,
                                "Left assignable operand shall point to the type with all qualifications from right"));
    } else if (target_type->tag == KEFIR_AST_TYPE_VA_LIST) {
        REQUIRE(value_type->tag == KEFIR_AST_TYPE_VA_LIST,
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Both assignable operands shall be va lists"));
    } else {
        REQUIRE(target_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Left assignable operand shall be a pointer"));
        REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(value_type) && constant_expression,
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Right assignable operand shall be NULL pointer"));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_node_assignable(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_node_base *node,
                                         const struct kefir_ast_type *target_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid value AST node"));
    REQUIRE(target_type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid target AST type"));

    const struct kefir_ast_type *value_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->properties.type);
    REQUIRE_OK(kefir_ast_type_assignable(mem, context, value_type,
                                         node->properties.expression_props.constant_expression, target_type));
    return KEFIR_OK;
}
