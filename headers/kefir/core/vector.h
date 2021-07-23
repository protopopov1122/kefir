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

#ifndef KEFIR_CORE_VECTOR_H_
#define KEFIR_CORE_VECTOR_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef struct kefir_vector {
    kefir_size_t element_size;
    void *content;
    kefir_size_t length;
    kefir_size_t capacity;
} kefir_vector_t;

kefir_result_t kefir_vector_init(struct kefir_vector *, kefir_size_t, void *, kefir_size_t);
kefir_size_t kefir_vector_length(const struct kefir_vector *);
kefir_size_t kefir_vector_available(const struct kefir_vector *);
void *kefir_vector_at(const struct kefir_vector *, kefir_size_t);
kefir_result_t kefir_vector_append(struct kefir_vector *, const void *);
kefir_result_t kefir_vector_copy(struct kefir_vector *, void *, kefir_size_t);
kefir_result_t kefir_vector_extend(struct kefir_vector *, kefir_size_t);
kefir_result_t kefir_vector_alloc(struct kefir_mem *, kefir_size_t, kefir_size_t, struct kefir_vector *);
kefir_result_t kefir_vector_realloc(struct kefir_mem *, kefir_size_t, struct kefir_vector *);
kefir_result_t kefir_vector_free(struct kefir_mem *, struct kefir_vector *);

#endif
