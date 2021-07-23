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

#ifndef KEFIR_CORE_MEM_H_
#define KEFIR_CORE_MEM_H_

#include "kefir/core/basic-types.h"

typedef struct kefir_mem {
    void *(*malloc)(struct kefir_mem *, kefir_size_t);
    void *(*calloc)(struct kefir_mem *, kefir_size_t, kefir_size_t);
    void *(*realloc)(struct kefir_mem *, void *, kefir_size_t);
    void (*free)(struct kefir_mem *, void *);
    void *data;
} kefir_mem_t;

#define KEFIR_MALLOC(mem, size) ((mem)->malloc((mem), (size)))
#define KEFIR_CALLOC(mem, num, size) ((mem)->calloc((mem), (num), (size)))
#define KEFIR_REALLOC(mem, ptr, size) ((mem)->realloc((mem), (ptr), (size)))
#define KEFIR_FREE(mem, ptr) ((mem)->free((mem), (ptr)))

#endif
