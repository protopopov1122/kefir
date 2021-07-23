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
#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/module.h"
#include "kefir/ast-translator/scope/scoped_identifier.h"

static kefir_result_t translate_object_identifier(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                  struct kefir_irbuilder_block *builder, const char *identifier,
                                                  const struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder, identifier, scoped_identifier));
    REQUIRE_OK(
        kefir_ast_translator_load_value(scoped_identifier->object.type, context->ast_context->type_traits, builder));
    return KEFIR_OK;
}

static kefir_result_t translate_enum_constant(struct kefir_irbuilder_block *builder,
                                              const struct kefir_ast_type_traits *type_traits,
                                              const struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(scoped_identifier->enum_constant.type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Enum constant cannot have non-integral type"));

    kefir_bool_t signedness;
    REQUIRE_OK(kefir_ast_type_is_signed(type_traits, scoped_identifier->enum_constant.type, &signedness));

    if (!signedness) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(
            builder, KEFIR_IROPCODE_PUSHU64, (kefir_uint64_t) scoped_identifier->enum_constant.value->value.integer));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(
            builder, KEFIR_IROPCODE_PUSHI64, (kefir_int64_t) scoped_identifier->enum_constant.value->value.integer));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_function_identifier(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder, const char *identifier) {
    REQUIRE_OK(kefir_ast_translator_function_lvalue(mem, context, builder, identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_identifier_node(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                   struct kefir_irbuilder_block *builder,
                                                   const struct kefir_ast_identifier *node) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier node"));

    const struct kefir_ast_scoped_identifier *scoped_identifier = node->base.properties.expression_props.scoped_id;
    switch (scoped_identifier->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            REQUIRE_OK(translate_object_identifier(mem, context, builder, node->identifier, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
            REQUIRE_OK(translate_enum_constant(builder, context->ast_context->type_traits, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            REQUIRE_OK(translate_function_identifier(mem, context, builder,
                                                     scoped_identifier->function.type->function_type.identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate type information");

        case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "AST label identifiers are not implemented yet");
    }
    return KEFIR_OK;
}
