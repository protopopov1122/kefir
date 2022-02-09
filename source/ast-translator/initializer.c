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

#include "kefir/ast-translator/initializer.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast/initializer_traversal.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/type.h"

struct traversal_param {
    struct kefir_mem *mem;
    struct kefir_ast_translator_context *context;
    struct kefir_irbuilder_block *builder;
    struct kefir_ast_translator_type *translator_type;
};

static kefir_result_t zero_type(struct kefir_irbuilder_block *builder, kefir_id_t ir_type_id,
                                const struct kefir_ast_type_layout *type_layout) {
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_BZERO, ir_type_id, type_layout->value));
    return KEFIR_OK;
}

static kefir_result_t translate_address(const struct kefir_ast_translator_type *translator_type,
                                        const struct kefir_ast_designator *designator,
                                        struct kefir_irbuilder_block *builder) {
    kefir_size_t offset = 0;
    struct kefir_ast_type_layout *layout = NULL;
    if (designator != NULL) {
        REQUIRE_OK(kefir_ast_type_layout_resolve_offset(translator_type->object.layout, designator, &layout, &offset));
    } else {
        layout = translator_type->object.layout;
    }

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    if (offset > 0) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD1, offset));
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_scalar(const struct kefir_ast_designator *designator,
                                      struct kefir_ast_node_base *expression, void *payload) {
    REQUIRE(expression != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST expression node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    struct kefir_ast_type_layout *type_layout = NULL;
    if (designator != NULL) {
        REQUIRE_OK(
            kefir_ast_type_layout_resolve(param->translator_type->object.layout, designator, &type_layout, NULL, NULL));
    } else {
        type_layout = param->translator_type->object.layout;
    }

    if (designator != NULL) {
        REQUIRE_OK(translate_address(param->translator_type, designator, param->builder));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(param->builder, KEFIR_IROPCODE_PICK, 0));
    }

    REQUIRE_OK(kefir_ast_translate_expression(param->mem, expression, param->builder, param->context));

    const struct kefir_ast_type *expr_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(
        param->mem, param->context->ast_context->type_bundle, expression->properties.type);
    REQUIRE(expr_type != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to perform lvalue conversions"));
    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(expr_type)) {
        REQUIRE_OK(kefir_ast_translate_typeconv(param->builder, param->context->ast_context->type_traits, expr_type,
                                                type_layout->type));
    }
    REQUIRE_OK(kefir_ast_translator_store_layout_value(param->mem, param->context, param->builder,
                                                       param->translator_type->object.ir_type, type_layout));
    return KEFIR_OK;
}

static kefir_result_t traverse_string_literal(const struct kefir_ast_designator *designator,
                                              struct kefir_ast_node_base *expression,
                                              kefir_ast_string_literal_type_t type, const void *string,
                                              kefir_size_t length, void *payload) {
    UNUSED(type);
    UNUSED(string);
    REQUIRE(expression != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST expression node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    struct kefir_ast_type_layout *type_layout = NULL;
    if (designator != NULL) {
        REQUIRE_OK(
            kefir_ast_type_layout_resolve(param->translator_type->object.layout, designator, &type_layout, NULL, NULL));
    } else {
        type_layout = param->translator_type->object.layout;
    }

    REQUIRE_OK(translate_address(param->translator_type, designator, param->builder));
    REQUIRE_OK(zero_type(param->builder, param->translator_type->object.ir_type_id, type_layout));
    const struct kefir_ast_type *array_type = kefir_ast_type_array(
        param->mem, param->context->ast_context->type_bundle, type_layout->type->array_type.element_type,
        kefir_ast_constant_expression_integer(param->mem, length), NULL);
    REQUIRE_OK(kefir_ast_translate_expression(param->mem, expression, param->builder, param->context));

    REQUIRE_OK(kefir_ast_translator_store_value(
        param->mem, KEFIR_AST_TYPE_SAME(type_layout->type, array_type) ? type_layout->type : array_type, param->context,
        param->builder));
    return KEFIR_OK;
}

static kefir_result_t traverse_aggregate_union(const struct kefir_ast_designator *designator,
                                               const struct kefir_ast_initializer *initializer, void *payload) {
    UNUSED(initializer);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    struct kefir_ast_type_layout *type_layout = NULL;
    if (designator != NULL) {
        REQUIRE_OK(
            kefir_ast_type_layout_resolve(param->translator_type->object.layout, designator, &type_layout, NULL, NULL));
    } else {
        type_layout = param->translator_type->object.layout;
    }

    REQUIRE_OK(zero_type(param->builder, param->translator_type->object.ir_type_id, type_layout));
    return KEFIR_OK;
}

static kefir_result_t traverse_initializer_list(const struct kefir_ast_designator *designator,
                                                const struct kefir_ast_initializer *initializer, void *payload) {
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST initializer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    struct kefir_ast_type_layout *type_layout = NULL;
    if (designator != NULL) {
        REQUIRE_OK(
            kefir_ast_type_layout_resolve(param->translator_type->object.layout, designator, &type_layout, NULL, NULL));
    } else {
        type_layout = param->translator_type->object.layout;
    }

    REQUIRE_OK(translate_address(param->translator_type, designator, param->builder));
    REQUIRE_OK(
        kefir_ast_translate_initializer(param->mem, param->context, param->builder, type_layout->type, initializer));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(param->builder, KEFIR_IROPCODE_POP, 0));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_initializer(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                               struct kefir_irbuilder_block *builder, const struct kefir_ast_type *type,
                                               const struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST initializer"));

    struct traversal_param param = {
        .mem = mem,
        .context = context,
        .builder = builder,
    };
    REQUIRE_OK(
        kefir_ast_translator_type_new(mem, context->environment, context->module, type, 0, &param.translator_type));

    struct kefir_ast_initializer_traversal initializer_traversal;
    KEFIR_AST_INITIALIZER_TRAVERSAL_INIT(&initializer_traversal);
    initializer_traversal.visit_value = traverse_scalar;
    initializer_traversal.visit_string_literal = traverse_string_literal;
    initializer_traversal.begin_struct_union = traverse_aggregate_union;
    initializer_traversal.begin_array = traverse_aggregate_union;
    initializer_traversal.visit_initializer_list = traverse_initializer_list;
    initializer_traversal.payload = &param;

    kefir_result_t res =
        kefi_ast_traverse_initializer(mem, context->ast_context, initializer, type, &initializer_traversal);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_type_free(mem, param.translator_type);
        return res;
    });
    REQUIRE_OK(kefir_ast_translator_type_free(mem, param.translator_type));
    return KEFIR_OK;
}
