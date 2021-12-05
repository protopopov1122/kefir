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

#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t resolve_vararg(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder, const struct kefir_ast_node_base *vararg) {
    if (vararg->properties.type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, vararg, builder, context));
    } else {
        REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, vararg));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_builtin_node(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                struct kefir_irbuilder_block *builder,
                                                const struct kefir_ast_builtin *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST builtin node"));

    const struct kefir_list_entry *iter = kefir_list_head(&node->arguments);
    switch (node->builtin) {
        case KEFIR_AST_BUILTIN_VA_START: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, vararg, iter->value);
            REQUIRE_OK(resolve_vararg(mem, context, builder, vararg));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_VARARG_START, 0));
        } break;

        case KEFIR_AST_BUILTIN_VA_END: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, vararg, iter->value);
            REQUIRE_OK(resolve_vararg(mem, context, builder, vararg));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_VARARG_END, 0));
        } break;

        case KEFIR_AST_BUILTIN_VA_ARG: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, vararg, iter->value);
            REQUIRE_OK(resolve_vararg(mem, context, builder, vararg));

            kefir_list_next(&iter);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, type_arg, iter->value);
            const struct kefir_ast_translator_resolved_type *cached_type = NULL;
            REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_cache.resolver,
                                                                       context->environment, context->module,
                                                                       type_arg->properties.type, 0, &cached_type));
            REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                    KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected cached type to be an object"));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(
                builder, KEFIR_IROPCODE_VARARG_GET, cached_type->object.ir_type_id, cached_type->object.layout->value));
            if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(cached_type->type)) {
                REQUIRE_OK(kefir_ast_translate_typeconv_normalize(builder, context->ast_context->type_traits,
                                                                  cached_type->type));
            }
        } break;

        case KEFIR_AST_BUILTIN_VA_COPY: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, dst_vararg, iter->value);
            REQUIRE_OK(resolve_vararg(mem, context, builder, dst_vararg));

            kefir_list_next(&iter);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, src_vararg, iter->value);
            REQUIRE_OK(resolve_vararg(mem, context, builder, src_vararg));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_VARARG_COPY, 0));
        } break;

        case KEFIR_AST_BUILTIN_ALLOCA: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, size, iter->value);
            REQUIRE_OK(kefir_ast_translate_expression(mem, size, builder, context));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64, 0));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ALLOCA, 1));
        } break;

        case KEFIR_AST_BUILTIN_ALLOCA_WITH_ALIGN:
        case KEFIR_AST_BUILTIN_ALLOCA_WITH_ALIGN_AND_MAX: {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, size, iter->value);
            REQUIRE_OK(kefir_ast_translate_expression(mem, size, builder, context));
            kefir_list_next(&iter);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, alignment, iter->value);
            REQUIRE_OK(kefir_ast_translate_expression(mem, alignment, builder, context));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_ALLOCA, 1));
        } break;
    }
    return KEFIR_OK;
}
