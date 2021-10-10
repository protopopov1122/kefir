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

#define INTERNAL
#include "./definitions.h"

size_t total_strlen(const char *str1, const char *str2) {
    size_t *sz = __builtin_alloca_with_align_and_max(sizeof(size_t), 32, 100);
    if (((uintptr_t) sz) % 32 != 0) {
        return (size_t) -1;
    }
    size_t sz1 = strlen(str1);
    size_t sz2 = strlen(str2);
    char *result = alloca(sz1 + sz2 + 1);
    strcpy(result, str1);
    strcat(result, str2);
    char *result2 = __builtin_alloca_with_align(sz1 + sz2 + 1, 16);
    if (((uintptr_t) result2) % 16 != 0) {
        return (size_t) -1;
    }
    strcpy(result2, result);
    *sz = strlen(result2);
    return *sz;
}
