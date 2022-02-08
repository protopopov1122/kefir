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

#ifndef KEFIR_AST_TYPE_CONV_H_
#define KEFIR_AST_TYPE_CONV_H_

#include "kefir/ast/type.h"

const struct kefir_ast_type *kefir_ast_type_int_promotion(const struct kefir_ast_type_traits *,
                                                          const struct kefir_ast_type *,
                                                          struct kefir_ast_bitfield_properties);
const struct kefir_ast_type *kefir_ast_type_common_arithmetic(const struct kefir_ast_type_traits *,
                                                              const struct kefir_ast_type *,
                                                              struct kefir_ast_bitfield_properties,
                                                              const struct kefir_ast_type *,
                                                              struct kefir_ast_bitfield_properties);

const struct kefir_ast_type *kefir_ast_type_function_default_argument_promotion(const struct kefir_ast_type_traits *,
                                                                                const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_function_default_argument_convertion_promotion(
    struct kefir_mem *, struct kefir_ast_type_bundle *, const struct kefir_ast_type_traits *,
    const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_lvalue_conversion(const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_array_conversion(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                             const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_va_list_conversion(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                               const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_function_conversion(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                                const struct kefir_ast_type *);

typedef enum kefir_ast_type_conv_expression_wrapper_param {
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_NONE = 0u,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_LVALUE = 1u,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ARRAY = 1u << 1,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_FUNCTION = 1u << 2,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_VA_LIST = 1u << 3,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ALL = (1u << 4) - 1u
} kefir_ast_type_conv_expression_wrapper_param_t;

const struct kefir_ast_type *kefir_ast_type_conv_expression_wrapper(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                                    const struct kefir_ast_type *, kefir_int_t);

#define KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, type_bundle, type)       \
    kefir_ast_type_conv_expression_wrapper((mem), (type_bundle), (type), \
                                           KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ALL)

const struct kefir_ast_type *kefir_ast_type_conv_adjust_function_parameter(struct kefir_mem *,
                                                                           struct kefir_ast_type_bundle *,
                                                                           const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_conv_unwrap_enumeration(const struct kefir_ast_type *);

#endif
