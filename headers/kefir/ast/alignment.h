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

#ifndef KEFIR_AST_ALIGNMENT_H_
#define KEFIR_AST_ALIGNMENT_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/context.h"
#include "kefir/ast/constants.h"

// Forward declaration
typedef struct kefir_ast_type kefir_ast_type_t;

typedef enum kefir_ast_alignment_class {
    KEFIR_AST_ALIGNMENT_DEFAULT,
    KEFIR_AST_ALIGNMENT_AS_TYPE,
    KEFIR_AST_ALIGNMENT_AS_CONST_EXPR
} kefir_ast_alignment_class_t;

typedef struct kefir_ast_alignment {
    kefir_ast_alignment_class_t klass;
    kefir_size_t value;
    union {
        const struct kefir_ast_type *type;
        struct kefir_ast_constant_expression *const_expr;
    };
} kefir_ast_alignment_t;

#define KEFIR_AST_DEFAULT_ALIGNMENT 0

struct kefir_ast_alignment *kefir_ast_alignment_default(struct kefir_mem *);

struct kefir_ast_alignment *kefir_ast_alignment_as_type(struct kefir_mem *, const struct kefir_ast_type *);

struct kefir_ast_alignment *kefir_ast_alignment_const_expression(struct kefir_mem *,
                                                                 struct kefir_ast_constant_expression *);

kefir_result_t kefir_ast_alignment_free(struct kefir_mem *, struct kefir_ast_alignment *);

kefir_result_t kefir_ast_alignment_evaluate(struct kefir_mem *, const struct kefir_ast_context *,
                                            struct kefir_ast_alignment *);

#endif
