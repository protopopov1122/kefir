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

#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"

const struct kefir_ast_type *kefir_ast_translator_normalize_type(const struct kefir_ast_type *original) {
    REQUIRE(original != NULL, NULL);

    const struct kefir_ast_type *unqualified = kefir_ast_unqualified_type(original);
    if (unqualified->tag == KEFIR_AST_TYPE_ENUMERATION) {
        unqualified = kefir_ast_unqualified_type(unqualified->enumeration_type.underlying_type);
    }
    return unqualified;
}
