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

typedef enum kefir_preprocessor_macro_type {
    KEFIR_PREPROCESSOR_MACRO_OBJECT,
    KEFIR_PREPROCESSOR_MACRO_FUNCTION
} kefir_preprocessor_macro_type_t;

typedef struct kefir_preprocessor_macro {
    const char *identifier;
    kefir_preprocessor_macro_type_t type;

    kefir_result_t (*argc)(const struct kefir_preprocessor_macro *, kefir_size_t *, kefir_bool_t *);
    kefir_result_t (*apply)(struct kefir_mem *, const struct kefir_preprocessor_macro *, const struct kefir_list *,
                            struct kefir_token_buffer *);
    void *payload;
} kefir_preprocessor_macro_t;

typedef struct kefir_preprocessor_macro_scope {
    kefir_result_t (*locate)(const struct kefir_preprocessor_macro_scope *, const char *,
                             const struct kefir_preprocessor_macro **);
    void *payload;
} kefir_preprocessor_macro_scope_t;

typedef struct kefir_preprocessor_user_macro {
    struct kefir_preprocessor_macro macro;
    struct kefir_list parameters;
    kefir_bool_t vararg;
    struct kefir_token_buffer replacement;
} kefir_preprocessor_user_macro_t;

struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new_object(struct kefir_mem *,
                                                                               struct kefir_symbol_table *,
                                                                               const char *);
struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new_function(struct kefir_mem *,
                                                                                 struct kefir_symbol_table *,
                                                                                 const char *);
kefir_result_t kefir_preprocessor_user_macro_free(struct kefir_mem *, struct kefir_preprocessor_user_macro *);

typedef struct kefir_preprocessor_user_macro_scope {
    struct kefir_preprocessor_macro_scope scope;
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
