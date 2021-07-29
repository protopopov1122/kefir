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

#ifndef KEFIR_AST_TRANSLATOR_TYPE_CACHE_H_
#define KEFIR_AST_TRANSLATOR_TYPE_CACHE_H_

#include "kefir/core/hashtree.h"
#include "kefir/ast-translator/type_resolver.h"

typedef struct kefir_ast_translator_type_cache {
    struct kefir_ast_translator_type_resolver resolver;
    const struct kefir_ast_translator_type_resolver *parent_resolver;
    struct kefir_hashtree cache;
} kefir_ast_translator_type_cache_t;

kefir_result_t kefir_ast_translator_type_cache_init(struct kefir_ast_translator_type_cache *,
                                                    const struct kefir_ast_translator_type_resolver *);

#endif
