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

#ifndef KEFIR_LEXER_LEXER_H_
#define KEFIR_LEXER_LEXER_H_

#include "kefir/lexer/lexem.h"
#include "kefir/lexer/buffer.h"
#include "kefir/lexer/source_cursor.h"
#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/core/trie.h"
#include "kefir/lexer/context.h"
#include "kefir/util/json.h"

typedef struct kefir_lexer {
    struct kefir_symbol_table *symbols;
    struct kefir_lexer_source_cursor *cursor;
    const struct kefir_lexer_context *context;

    struct kefir_trie punctuators;
    struct kefir_trie keywords;
} kefir_lexer_t;

typedef kefir_result_t (*kefir_lexer_callback_fn_t)(struct kefir_mem *, struct kefir_lexer *, void *);

kefir_result_t kefir_lexer_init(struct kefir_mem *, struct kefir_lexer *, struct kefir_symbol_table *,
                                struct kefir_lexer_source_cursor *, const struct kefir_lexer_context *);
kefir_result_t kefir_lexer_free(struct kefir_mem *, struct kefir_lexer *);
kefir_result_t kefir_lexer_apply(struct kefir_mem *, struct kefir_lexer *, kefir_lexer_callback_fn_t, void *);
kefir_result_t kefir_lexer_next(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_cursor_match_whitespace(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_cursor_next_universal_character(struct kefir_lexer_source_cursor *, kefir_char32_t *);
kefir_result_t kefir_lexer_cursor_next_escape_sequence(struct kefir_lexer_source_cursor *, kefir_char32_t *);

kefir_result_t kefir_lexer_init_punctuators(struct kefir_mem *, struct kefir_lexer *);
kefir_result_t kefir_lexer_match_punctuator(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_init_keywords(struct kefir_mem *, struct kefir_lexer *);
kefir_result_t kefir_lexer_get_keyword(struct kefir_lexer *, const kefir_char32_t *, kefir_keyword_token_t *);
kefir_result_t kefir_lexer_match_identifier_or_keyword(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_next_narrow_string_literal(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_next_unicode8_string_literal(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_next_unicode16_string_literal(struct kefir_mem *, struct kefir_lexer *,
                                                         struct kefir_token *);
kefir_result_t kefir_lexer_next_unicode32_string_literal(struct kefir_mem *, struct kefir_lexer *,
                                                         struct kefir_token *);
kefir_result_t kefir_lexer_next_wide_string_literal(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_string_literal(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_match_floating_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_integer_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_character_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);
kefir_result_t kefir_lexer_match_constant(struct kefir_mem *, struct kefir_lexer *, struct kefir_token *);

kefir_result_t kefir_lexer_populate_buffer(struct kefir_mem *, struct kefir_token_buffer *, struct kefir_lexer *);

#endif
