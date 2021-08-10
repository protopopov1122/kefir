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

static _Thread_local kefir_size_t next_error_index = 0;
static _Thread_local struct kefir_error error_stack[KEFIR_ERROR_STACK_SIZE];

const struct kefir_error *kefir_current_error() {
    if (next_error_index != 0) {
        return &error_stack[next_error_index - 1];
    } else {
        return NULL;
    }
}

void kefir_clear_error() {
    next_error_index = 0;
}

kefir_result_t kefir_set_error(kefir_result_t code, const char *message, const char *file, unsigned int line) {
    if (next_error_index == KEFIR_ERROR_STACK_SIZE || kefir_result_get_category(code) == KEFIR_RESULT_CATEGORY_NORMAL) {
        return code;
    }

    if (next_error_index > 0 &&
        kefir_result_get_category(error_stack[next_error_index - 1].code) == KEFIR_RESULT_CATEGORY_WARNING) {
        // Override warning
        next_error_index--;
    }

    struct kefir_error *current_error = &error_stack[next_error_index];
    current_error->code = code;
    current_error->message = message;
    current_error->file = file;
    current_error->line = line;
    if (next_error_index != 0) {
        current_error->prev_error = &error_stack[next_error_index - 1];
    } else {
        current_error->prev_error = NULL;
    }
    current_error->error_overflow = ++next_error_index == KEFIR_ERROR_STACK_SIZE;
    return code;
}
