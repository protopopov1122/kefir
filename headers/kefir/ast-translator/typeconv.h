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

#ifndef KEFIR_AST_TRANSLATOR_TYPECONV_H_
#define KEFIR_AST_TRANSLATOR_TYPECONV_H_

#include "kefir/ast/type.h"
#include "kefir/ir/builder.h"

kefir_result_t kefir_ast_translate_typeconv(struct kefir_irbuilder_block *, const struct kefir_ast_type_traits *,
                                            const struct kefir_ast_type *, const struct kefir_ast_type *);

#endif
