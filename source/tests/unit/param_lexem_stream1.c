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

#include "kefir/parser/lexem_stream.h"
#include "kefir/test/unit_test.h"
#include "kefir/test/util.h"
#include <string.h>

DEFINE_CASE(parser_token_array_stream1, "Parser - token array stream") {
    struct kefir_token TOKENS[10];
    ASSERT_OK(kefir_token_new_constant_char('X', &TOKENS[0]));
    ASSERT_OK(kefir_token_new_identifier(NULL, NULL, "ident1", &TOKENS[1]));
    ASSERT_OK(kefir_token_new_keyword(KEFIR_KEYWORD_ALIGNAS, &TOKENS[2]));
    ASSERT_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_ARROW, &TOKENS[3]));

    struct kefir_token_array_stream array_stream;
    ASSERT_OK(kefir_token_array_stream_init(&array_stream, TOKENS, 4));
    struct kefir_token_stream *stream = &array_stream.stream;

    struct kefir_token token;
    ASSERT_OK(stream->next(&kft_mem, stream, &token));
    ASSERT(token.klass == KEFIR_TOKEN_CONSTANT);
    ASSERT(token.constant.type == KEFIR_CONSTANT_TOKEN_CHAR);
    ASSERT(token.constant.character == 'X');
    ASSERT_OK(kefir_token_free(&kft_mem, &token));

    ASSERT_OK(stream->next(&kft_mem, stream, &token));
    ASSERT(token.klass == KEFIR_TOKEN_IDENTIFIER);
    ASSERT(strcmp(token.identifier, "ident1") == 0);
    ASSERT_OK(kefir_token_free(&kft_mem, &token));

    ASSERT_OK(stream->next(&kft_mem, stream, &token));
    ASSERT(token.klass == KEFIR_TOKEN_KEYWORD);
    ASSERT(token.keyword == KEFIR_KEYWORD_ALIGNAS);
    ASSERT_OK(kefir_token_free(&kft_mem, &token));

    ASSERT_OK(stream->next(&kft_mem, stream, &token));
    ASSERT(token.klass == KEFIR_TOKEN_PUNCTUATOR);
    ASSERT(token.punctuator == KEFIR_PUNCTUATOR_RIGHT_ARROW);
    ASSERT_OK(kefir_token_free(&kft_mem, &token));

    ASSERT(stream->next(&kft_mem, stream, &token) == KEFIR_ITERATOR_END);

    ASSERT_OK(stream->free(&kft_mem, stream));

    for (int i = 0; i < 4; i++) {
        ASSERT_OK(kefir_token_free(&kft_mem, &TOKENS[i]));
    }
}
END_CASE
