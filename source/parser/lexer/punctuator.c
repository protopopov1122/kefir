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

#include "kefir/parser/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static const struct Punctuators {
    kefir_punctuator_token_t punctuator;
    const kefir_char32_t *literal;
} PUNCTUATORS[] = {{KEFIR_PUNCTUATOR_LEFT_BRACKET, U"["},
                   {KEFIR_PUNCTUATOR_RIGHT_BRACKET, U"]"},
                   {KEFIR_PUNCTUATOR_LEFT_PARENTHESE, U"("},
                   {KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, U")"},
                   {KEFIR_PUNCTUATOR_LEFT_BRACE, U"{"},
                   {KEFIR_PUNCTUATOR_RIGHT_BRACE, U"}"},
                   {KEFIR_PUNCTUATOR_DOT, U"."},
                   {KEFIR_PUNCTUATOR_RIGHT_ARROW, U"->"},
                   {KEFIR_PUNCTUATOR_DOUBLE_PLUS, U"++"},
                   {KEFIR_PUNCTUATOR_DOUBLE_MINUS, U"--"},
                   {KEFIR_PUNCTUATOR_AMPERSAND, U"&"},
                   {KEFIR_PUNCTUATOR_STAR, U"*"},
                   {KEFIR_PUNCTUATOR_PLUS, U"+"},
                   {KEFIR_PUNCTUATOR_MINUS, U"-"},
                   {KEFIR_PUNCTUATOR_TILDE, U"~"},
                   {KEFIR_PUNCTUATOR_EXCLAMATION_MARK, U"!"},
                   {KEFIR_PUNCTUATOR_SLASH, U"/"},
                   {KEFIR_PUNCTUATOR_PERCENT, U"%"},
                   {KEFIR_PUNCTUATOR_LEFT_SHIFT, U"<<"},
                   {KEFIR_PUNCTUATOR_RIGHT_SHIFT, U">>"},
                   {KEFIR_PUNCTUATOR_LESS_THAN, U"<"},
                   {KEFIR_PUNCTUATOR_GREATER_THAN, U">"},
                   {KEFIR_PUNCTUATOR_LESS_OR_EQUAL, U"<="},
                   {KEFIR_PUNCTUATOR_GREATER_OR_EQUAL, U">="},
                   {KEFIR_PUNCTUATOR_EQUAL, U"=="},
                   {KEFIR_PUNCTUATOR_NOT_EQUAL, U"!="},
                   {KEFIR_PUNCTUATOR_CARET, U"^"},
                   {KEFIR_PUNCTUATOR_VBAR, U"|"},
                   {KEFIR_PUNCTUATOR_DOUBLE_AMPERSAND, U"&&"},
                   {KEFIR_PUNCTUATOR_DOUBLE_VBAR, U"||"},
                   {KEFIR_PUNCTUATOR_QUESTION_MARK, U"?"},
                   {KEFIR_PUNCTUATOR_COLON, U":"},
                   {KEFIR_PUNCTUATOR_SEMICOLON, U";"},
                   {KEFIR_PUNCTUATOR_ELLIPSIS, U"..."},
                   {KEFIR_PUNCTUATOR_ASSIGN, U"="},
                   {KEFIR_PUNCTUATOR_ASSIGN_MULTIPLY, U"*="},
                   {KEFIR_PUNCTUATOR_ASSIGN_DIVIDE, U"/="},
                   {KEFIR_PUNCTUATOR_ASSIGN_MODULO, U"%="},
                   {KEFIR_PUNCTUATOR_ASSIGN_ADD, U"+="},
                   {KEFIR_PUNCTUATOR_ASSIGN_SUBTRACT, U"-="},
                   {KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT, U"<<="},
                   {KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT, U">>="},
                   {KEFIR_PUNCTUATOR_ASSIGN_AND, U"&="},
                   {KEFIR_PUNCTUATOR_ASSIGN_XOR, U"^="},
                   {KEFIR_PUNCTUATOR_ASSIGN_OR, U"|="},
                   {KEFIR_PUNCTUATOR_COMMA, U","},
                   {KEFIR_PUNCTUATOR_HASH, U"#"},
                   {KEFIR_PUNCTUATOR_DOUBLE_HASH, U"##"},
                   {KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACKET, U"<:"},
                   {KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACKET, U":>"},
                   {KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACE, U"<%"},
                   {KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACE, U"%>"},
                   {KEFIR_PUNCTUATOR_DIGRAPH_HASH, U"%:"},
                   {KEFIR_PUNCTUATOR_DIGRAPH_DOUBLE_HASH, U"%:%:"}};
static const kefir_size_t PUNCTUATORS_LENGTH = sizeof(PUNCTUATORS) / sizeof(PUNCTUATORS[0]);

#define PUNCTUATOR_NONE (~((kefir_trie_value_t) 0))

static kefir_result_t insert_punctuator(struct kefir_mem *mem, struct kefir_trie *trie, const kefir_char32_t *literal,
                                        kefir_punctuator_token_t punctuator) {
    struct kefir_trie_vertex *vertex = NULL;
    if (literal[1] == U'\0') {
        kefir_result_t res = kefir_trie_at(trie, (kefir_trie_key_t) literal[0], &vertex);
        if (res == KEFIR_NOT_FOUND) {
            REQUIRE_OK(kefir_trie_insert_vertex(mem, trie, (kefir_trie_key_t) literal[0],
                                                (kefir_trie_value_t) punctuator, &vertex));
        } else {
            REQUIRE_OK(res);
            REQUIRE(vertex->node.value == PUNCTUATOR_NONE,
                    KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Punctuator clash in the trie"));
            vertex->node.value = (kefir_trie_value_t) punctuator;
        }
    } else {
        kefir_result_t res = kefir_trie_at(trie, (kefir_trie_key_t) literal[0], &vertex);
        if (res == KEFIR_NOT_FOUND) {
            REQUIRE_OK(kefir_trie_insert_vertex(mem, trie, (kefir_trie_key_t) literal[0], PUNCTUATOR_NONE, &vertex));
        } else {
            REQUIRE_OK(res);
        }
        REQUIRE_OK(insert_punctuator(mem, &vertex->node, literal + 1, punctuator));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_init_punctuators(struct kefir_mem *mem, struct kefir_lexer *lexer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));

    REQUIRE_OK(kefir_trie_init(&lexer->punctuators, PUNCTUATOR_NONE));
    for (kefir_size_t i = 0; i < PUNCTUATORS_LENGTH; i++) {
        kefir_result_t res =
            insert_punctuator(mem, &lexer->punctuators, PUNCTUATORS[i].literal, PUNCTUATORS[i].punctuator);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_trie_free(mem, &lexer->punctuators);
            return res;
        });
    }
    return KEFIR_OK;
}

static kefir_result_t match_punctuator(struct kefir_lexer_source_cursor *cursor, struct kefir_trie *trie,
                                       struct kefir_token *token) {
    struct kefir_trie_vertex *vertex = NULL;
    REQUIRE_OK(kefir_trie_at(trie, (kefir_trie_key_t) kefir_lexer_source_cursor_at(cursor, 0), &vertex));
    REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));

    kefir_result_t res = match_punctuator(cursor, &vertex->node, token);
    if (res == KEFIR_NOT_FOUND) {
        REQUIRE(vertex->node.value != PUNCTUATOR_NONE, KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match punctuator"));
        REQUIRE_OK(kefir_token_new_punctuator((kefir_punctuator_token_t) vertex->node.value, token));
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_result_t res = match_punctuator(lexer->cursor, &lexer->punctuators, token);
    if (res == KEFIR_NOT_FOUND) {
        res = KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match punctuator");
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_punctuator(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                            struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
