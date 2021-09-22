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

#ifndef KEFIR_PREPROCESSOR_FORMAT_H_
#define KEFIR_PREPROCESSOR_FORMAT_H_

#include "kefir/lexer/buffer.h"
#include <stdio.h>

typedef enum kefir_preprocessor_whitespace_format {
    KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_ORIGINAL,
    KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_SINGLE_SPACE
} kefir_preprocessor_whitespace_format_t;

kefir_result_t kefir_preprocessor_format(FILE *, const struct kefir_token_buffer *,
                                         kefir_preprocessor_whitespace_format_t);
kefir_result_t kefir_preprocessor_format_string(struct kefir_mem *, char **, kefir_size_t *,
                                                const struct kefir_token_buffer *,
                                                kefir_preprocessor_whitespace_format_t);

#endif
