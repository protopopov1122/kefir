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

#include "kefir/ast-translator/misc.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t sizeof_nonvla_type(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                         struct kefir_irbuilder_block *builder, const struct kefir_ast_type *type) {
    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env, type, &opaque_type));
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env, opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
        return res;
    });

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.size));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_sizeof(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                          struct kefir_irbuilder_block *builder, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    if (KEFIR_AST_TYPE_IS_VL_ARRAY(type)) {
        const struct kefir_ast_node_base *vlen = kefir_ast_type_get_top_variable_modificator(type);
        REQUIRE(vlen != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_STATE,
                                              "Unable to determine size of VLA with unspecified variable modifier"));
        REQUIRE_OK(kefir_ast_translate_sizeof(mem, context, builder, type->array_type.element_type));
        REQUIRE_OK(kefir_ast_translate_expression(mem, vlen, builder, context));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_IMUL, 0));
    } else {
        REQUIRE_OK(sizeof_nonvla_type(mem, context, builder, type));
    }
    return KEFIR_OK;
}

static kefir_result_t unwrap_vla_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                      const struct kefir_ast_type *type, const struct kefir_ast_type **result) {
    if (KEFIR_AST_TYPE_IS_VL_ARRAY(type)) {
        const struct kefir_ast_type *element_type = NULL;
        REQUIRE_OK(unwrap_vla_type(mem, context, type->array_type.element_type, &element_type));
        struct kefir_ast_constant_expression *len_expr = kefir_ast_constant_expression_integer(mem, 1);
        REQUIRE(len_expr != NULL,
                KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate integral constant expression"));
        const struct kefir_ast_type *array_type =
            kefir_ast_type_array(mem, context->type_bundle, element_type, len_expr, NULL);
        REQUIRE_ELSE(array_type != NULL, {
            kefir_ast_constant_expression_free(mem, len_expr);
            return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed toa allocate AST array type");
        });
        *result = array_type;
    } else {
        *result = type;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_alignof(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_type *base_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(base_type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    kefir_ast_target_environment_opaque_type_t opaque_type;
    struct kefir_ast_target_environment_object_info type_info;
    const struct kefir_ast_type *type = NULL;
    REQUIRE_OK(unwrap_vla_type(mem, context->ast_context, base_type, &type));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, &context->environment->target_env, type, &opaque_type));
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, &context->environment->target_env, opaque_type, NULL, &type_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type);
        return res;
    });

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64, type_info.alignment));
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, &context->environment->target_env, opaque_type));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_resolve_type_layout(struct kefir_irbuilder_block *builder, kefir_id_t type_id,
                                                        const struct kefir_ast_type_layout *layout,
                                                        const struct kefir_ast_type_layout *root) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout"));

    if (layout->parent != NULL && layout->parent != root) {
        REQUIRE_OK(kefir_ast_translator_resolve_type_layout(builder, type_id, layout->parent, root));
    }

    if (layout->properties.relative_offset != 0) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, type_id, layout->value));
    }
    return KEFIR_OK;
}
