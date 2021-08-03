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

#ifndef KEFIR_AST_TRANSLATOR_TYPE_CACHE_IMPL_H_
#define KEFIR_AST_TRANSLATOR_TYPE_CACHE_IMPL_H_

#include "kefir/ast-translator/type_cache.h"

typedef struct kefir_ast_translator_aligned_type_cache {
    kefir_size_t alignment;
    struct kefir_hashtree cache;
} kefir_ast_translator_aligned_type_cache_t;

kefir_result_t kefir_ast_translator_aligned_type_cache_init(struct kefir_ast_translator_aligned_type_cache *,
                                                            kefir_size_t);
kefir_result_t kefir_ast_translator_aligned_type_cache_free(struct kefir_mem *,
                                                            struct kefir_ast_translator_aligned_type_cache *);

kefir_result_t kefir_ast_translator_aligned_type_cache_at(struct kefir_ast_translator_aligned_type_cache *,
                                                          const struct kefir_ast_type *,
                                                          const struct kefir_ast_translator_resolved_type **);

kefir_result_t kefir_ast_translator_aligned_type_layout_insert(struct kefir_mem *,
                                                               struct kefir_ast_translator_aligned_type_cache *,
                                                               struct kefir_ast_translator_resolved_type *);

kefir_result_t kefir_ast_translator_type_cache_free(struct kefir_mem *, struct kefir_ast_translator_type_cache *);

kefir_result_t kefir_ast_translator_type_cache_at(struct kefir_ast_translator_type_cache *,
                                                  const struct kefir_ast_type *, kefir_size_t,
                                                  const struct kefir_ast_translator_resolved_type **);

kefir_result_t kefir_ast_translator_type_cache_insert(struct kefir_mem *, struct kefir_ast_translator_type_cache *,
                                                      struct kefir_ast_translator_resolved_type *);

kefir_result_t kefir_ast_translator_type_cache_insert_unowned_object(struct kefir_mem *,
                                                                     struct kefir_ast_translator_type_cache *,
                                                                     kefir_id_t, struct kefir_ir_type *,
                                                                     struct kefir_ast_type_layout *);

kefir_result_t kefir_ast_translator_type_cache_insert_unowned_function(
    struct kefir_mem *, struct kefir_ast_translator_type_cache *, struct kefir_ast_translator_function_declaration *);

kefir_result_t kefir_ast_translator_type_cache_generate_owned_object(
    struct kefir_mem *, const struct kefir_ast_type *, kefir_size_t, struct kefir_ast_translator_type_cache *,
    const struct kefir_ast_translator_environment *, struct kefir_ir_module *,
    const struct kefir_ast_translator_resolved_type **);

kefir_result_t kefir_ast_translator_type_cache_generate_owned_function(
    struct kefir_mem *, const struct kefir_ast_type *, struct kefir_ast_translator_type_cache *,
    const struct kefir_ast_translator_environment *, struct kefir_ast_type_bundle *,
    const struct kefir_ast_type_traits *, struct kefir_ir_module *, const struct kefir_ast_translator_resolved_type **);

#endif
