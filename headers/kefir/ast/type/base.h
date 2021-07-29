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

#ifndef KEFIR_AST_TYPE_BASE_H_
#define KEFIR_AST_TYPE_BASE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef struct kefir_ast_type kefir_ast_type_t;
typedef struct kefir_ast_type_bundle kefir_ast_type_bundle_t;

typedef enum kefir_ast_type_tag {
    KEFIR_AST_TYPE_VOID,
    KEFIR_AST_TYPE_SCALAR_BOOL,
    KEFIR_AST_TYPE_SCALAR_CHAR,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR,
    KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT,
    KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT,
    KEFIR_AST_TYPE_SCALAR_SIGNED_INT,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG,
    KEFIR_AST_TYPE_SCALAR_SIGNED_LONG,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG,
    KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG,
    KEFIR_AST_TYPE_SCALAR_FLOAT,
    KEFIR_AST_TYPE_SCALAR_DOUBLE,
    KEFIR_AST_TYPE_SCALAR_POINTER,
    KEFIR_AST_TYPE_ENUMERATION,
    KEFIR_AST_TYPE_STRUCTURE,
    KEFIR_AST_TYPE_UNION,
    KEFIR_AST_TYPE_ARRAY,
    KEFIR_AST_TYPE_FUNCTION,
    KEFIR_AST_TYPE_QUALIFIED
} kefir_ast_type_tag_t;

typedef struct kefir_ast_type_qualification {
    kefir_bool_t constant;
    kefir_bool_t restricted;
    kefir_bool_t volatile_type;
} kefir_ast_type_qualification_t;

#endif
