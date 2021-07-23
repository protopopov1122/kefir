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

#ifndef KEFIR_CORE_SYMBOL_TABLE_H_
#define KEFIR_CORE_SYMBOL_TABLE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/hashtree.h"

typedef struct kefir_symbol_table {
    struct kefir_hashtree symbols;
    struct kefir_hashtree symbol_identifiers;
    kefir_size_t next_id;
} kefir_symbol_table_t;

kefir_result_t kefir_symbol_table_init(struct kefir_symbol_table *);
kefir_result_t kefir_symbol_table_free(struct kefir_mem *, struct kefir_symbol_table *);
const char *kefir_symbol_table_insert(struct kefir_mem *, struct kefir_symbol_table *, const char *, kefir_id_t *);
const char *kefir_symbol_table_get(const struct kefir_symbol_table *, kefir_id_t);

#endif
