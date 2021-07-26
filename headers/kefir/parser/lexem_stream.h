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

#ifndef KEFIR_PARSER_LEXEM_STREAM_H_
#define KEFIR_PARSER_LEXEM_STREAM_H_

#include "kefir/core/mem.h"
#include "kefir/parser/lexem.h"

typedef struct kefir_token_stream {
    kefir_result_t (*next)(struct kefir_mem *, struct kefir_token_stream *, struct kefir_token *);
    kefir_result_t (*free)(struct kefir_mem *, struct kefir_token_stream *);
    void *payload;
} kefir_token_stream_t;

typedef struct kefir_token_array_stream {
    struct kefir_token_stream stream;
    const struct kefir_token *tokens;
    kefir_size_t index;
    kefir_size_t length;
} kefir_token_array_stream_t;

kefir_result_t kefir_token_array_stream_init(struct kefir_token_array_stream *, const struct kefir_token *,
                                             kefir_size_t);

#endif
