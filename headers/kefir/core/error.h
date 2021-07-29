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

#ifndef KEFIR_CORE_ERROR_H_
#define KEFIR_CORE_ERROR_H_

#include "kefir/core/basic-types.h"

typedef struct kefir_error {
    kefir_result_t code;
    const char *message;
    const char *file;
    unsigned int line;
} kefir_error_t;

const struct kefir_error *kefir_current_error();
void kefir_clear_error();
kefir_result_t kefir_set_error(kefir_result_t, const char *, const char *, unsigned int);

#define KEFIR_SET_ERROR(code, message) kefir_set_error((code), (message), __FILE__, __LINE__)

#endif
