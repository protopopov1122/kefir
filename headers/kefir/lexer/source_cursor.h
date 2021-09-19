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

#ifndef KEFIR_LEXER_SOURCE_CURSOR_H_
#define KEFIR_LEXER_SOURCE_CURSOR_H_

#include "kefir/lexer/base.h"
#include "kefir/core/basic-types.h"
#include "kefir/core/source_location.h"
#include <uchar.h>

typedef struct kefir_lexer_source_cursor {
    const char *content;
    kefir_size_t index;
    kefir_size_t length;
    mbstate_t mbstate;
    struct kefir_source_location location;
    kefir_char32_t newline_char;
} kefir_lexer_source_cursor_t;

typedef struct kefir_lexer_source_cursor_state {
    kefir_size_t index;
    mbstate_t mbstate;
    struct kefir_source_location location;
} kefir_lexer_source_cursor_state_t;

kefir_result_t kefir_lexer_source_cursor_init(struct kefir_lexer_source_cursor *, const char *, kefir_size_t,
                                              const char *);
kefir_char32_t kefir_lexer_source_cursor_at(const struct kefir_lexer_source_cursor *, kefir_size_t);
kefir_result_t kefir_lexer_source_cursor_next(struct kefir_lexer_source_cursor *, kefir_size_t);
kefir_result_t kefir_lexer_source_cursor_save(const struct kefir_lexer_source_cursor *,
                                              struct kefir_lexer_source_cursor_state *);
kefir_result_t kefir_lexer_source_cursor_restore(struct kefir_lexer_source_cursor *,
                                                 const struct kefir_lexer_source_cursor_state *);
kefir_result_t kefir_lexer_cursor_match_string(const struct kefir_lexer_source_cursor *, const kefir_char32_t *);

#endif
