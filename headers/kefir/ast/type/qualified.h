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

#ifndef KEFIR_AST_TYPE_QUALIFIED_H_
#define KEFIR_AST_TYPE_QUALIFIED_H_

#include "kefir/ast/type/base.h"

typedef struct kefir_ast_qualified_type {
    const struct kefir_ast_type *type;
    struct kefir_ast_type_qualification qualification;
} kefir_ast_qualified_type_t;

const struct kefir_ast_type *kefir_ast_type_qualified(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                      const struct kefir_ast_type *,
                                                      struct kefir_ast_type_qualification);

const struct kefir_ast_type *kefir_ast_unqualified_type(const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_zero_unqualified_type(const struct kefir_ast_type *);

kefir_result_t kefir_ast_type_retrieve_qualifications(struct kefir_ast_type_qualification *,
                                                      const struct kefir_ast_type *);

kefir_result_t kefir_ast_type_merge_qualifications(struct kefir_ast_type_qualification *,
                                                   const struct kefir_ast_type_qualification *,
                                                   const struct kefir_ast_type_qualification *);

#define KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(qualification) \
    (!(qualification)->constant && !(qualification)->restricted && !(qualification)->volatile_type)

#endif
