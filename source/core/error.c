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

#include "kefir/core/error.h"

static _Thread_local struct kefir_error current_error;

const struct kefir_error *kefir_current_error() {
    if (current_error.code != KEFIR_OK) {
        return &current_error;
    } else {
        return NULL;
    }
}

void kefir_clear_error() {
    current_error.code = KEFIR_OK;
}

kefir_result_t kefir_set_error(kefir_result_t code, const char *message, const char *file, unsigned int line) {
    current_error.code = code;
    current_error.message = message;
    current_error.file = file;
    current_error.line = line;
    return code;
}
