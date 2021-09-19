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

#ifndef kefir_preprocessor_user_macro_H_
#define kefir_preprocessor_user_macro_H_

#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/symbol_table.h"
#include "kefir/lexer/buffer.h"

typedef struct kefir_preprocessor_user_macro {
    const char *identifier;
    struct kefir_list parameters;
    kefir_bool_t vararg;
    struct kefir_token_buffer replacement;
} kefir_preprocessor_user_macro_t;

struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new(struct kefir_mem *, struct kefir_symbol_table *,
                                                                        const char *);
kefir_result_t kefir_preprocessor_user_macro_free(struct kefir_mem *, struct kefir_preprocessor_user_macro *);

typedef struct kefir_preprocessor_user_macro_scope {
    const struct kefir_preprocessor_user_macro_scope *parent;
    struct kefir_hashtree macros;
} kefir_preprocessor_user_macro_scope_t;

kefir_result_t kefir_preprocessor_user_macro_scope_init(const struct kefir_preprocessor_user_macro_scope *,
                                                        struct kefir_preprocessor_user_macro_scope *);
kefir_result_t kefir_preprocessor_user_macro_scope_free(struct kefir_mem *,
                                                        struct kefir_preprocessor_user_macro_scope *);
kefir_result_t kefir_preprocessor_user_macro_scope_insert(struct kefir_mem *,
                                                          struct kefir_preprocessor_user_macro_scope *,
                                                          struct kefir_preprocessor_user_macro *);
kefir_result_t kefir_preprocessor_user_macro_scope_at(const struct kefir_preprocessor_user_macro_scope *, const char *,
                                                      const struct kefir_preprocessor_user_macro **);

#endif
