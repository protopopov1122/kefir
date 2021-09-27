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

#ifndef KEFIR_AST_TYPE_FUNCTION_H_
#define KEFIR_AST_TYPE_FUNCTION_H_

#include "kefir/ast/type/base.h"
#include "kefir/ast/constants.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/optional.h"

typedef enum kefir_ast_function_type_mode {
    KEFIR_AST_FUNCTION_TYPE_PARAMETERS,
    KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS,
    KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY
} kefir_ast_function_type_mode_t;

typedef struct kefir_ast_function_type_parameter {
    const char *identifier;
    const struct kefir_ast_type *type;
    const struct kefir_ast_type *adjusted_type;
    KEFIR_OPTIONAL_TYPE(kefir_ast_scoped_identifier_storage_t) storage;
} kefir_ast_function_type_parameter_t;

typedef struct kefir_ast_function_type {
    const struct kefir_ast_type *return_type;
    kefir_ast_function_type_mode_t mode;
    struct kefir_list parameters;
    struct kefir_hashtree parameter_index;
    kefir_bool_t ellipsis;
} kefir_ast_function_type_t;

kefir_size_t kefir_ast_type_function_parameter_count(const struct kefir_ast_function_type *);

kefir_result_t kefir_ast_type_function_get_parameter(const struct kefir_ast_function_type *, kefir_size_t,
                                                     const struct kefir_ast_function_type_parameter **);

kefir_result_t kefir_ast_type_function_parameter(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                 struct kefir_ast_function_type *, const char *,
                                                 const struct kefir_ast_type *,
                                                 const kefir_ast_scoped_identifier_storage_t *);

kefir_result_t kefir_ast_type_function_ellipsis(struct kefir_ast_function_type *, kefir_bool_t);

const struct kefir_ast_type *kefir_ast_type_function(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                     const struct kefir_ast_type *, const char *,
                                                     struct kefir_ast_function_type **);

#endif
