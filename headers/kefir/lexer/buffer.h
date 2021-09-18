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

#ifndef KEFIR_LEXER_BUFFER_H_
#define KEFIR_LEXER_BUFFER_H_

#include "kefir/lexer/lexem.h"

typedef struct kefir_token_buffer {
    struct kefir_token *tokens;
    kefir_size_t length;
    kefir_size_t capacity;
} kefir_token_buffer_t;

kefir_result_t kefir_token_buffer_init(struct kefir_mem *, struct kefir_token_buffer *);
kefir_result_t kefir_token_buffer_free(struct kefir_mem *, struct kefir_token_buffer *);
kefir_result_t kefir_token_buffer_emplace(struct kefir_mem *, struct kefir_token_buffer *, struct kefir_token *);
kefir_result_t kefir_token_buffer_insert(struct kefir_mem *, struct kefir_token_buffer *, struct kefir_token_buffer *);

#endif
