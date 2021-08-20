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
#include "kefir/util/char32.h"

_Thread_local struct KeywordEntry {
    const kefir_char32_t *literal;
    kefir_keyword_token_t keyword;
} KEYWORDS[] = {{U"auto", KEFIR_KEYWORD_AUTO},
                {U"break", KEFIR_KEYWORD_BREAK},
                {U"case", KEFIR_KEYWORD_CASE},
                {U"char", KEFIR_KEYWORD_CHAR},
                {U"const", KEFIR_KEYWORD_CONST},
                {U"continue", KEFIR_KEYWORD_CONTINUE},
                {U"default", KEFIR_KEYWORD_DEFAULT},
                {U"do", KEFIR_KEYWORD_DO},
                {U"double", KEFIR_KEYWORD_DOUBLE},
                {U"else", KEFIR_KEYWORD_ELSE},
                {U"enum", KEFIR_KEYWORD_ENUM},
                {U"extern", KEFIR_KEYWORD_EXTERN},
                {U"float", KEFIR_KEYWORD_FLOAT},
                {U"for", KEFIR_KEYWORD_FOR},
                {U"goto", KEFIR_KEYWORD_GOTO},
                {U"if", KEFIR_KEYWORD_IF},
                {U"inline", KEFIR_KEYWORD_INLINE},
                {U"int", KEFIR_KEYWORD_INT},
                {U"long", KEFIR_KEYWORD_LONG},
                {U"register", KEFIR_KEYWORD_REGISTER},
                {U"restrict", KEFIR_KEYWORD_RESTRICT},
                {U"return", KEFIR_KEYWORD_RETURN},
                {U"short", KEFIR_KEYWORD_SHORT},
                {U"signed", KEFIR_KEYWORD_SIGNED},
                {U"sizeof", KEFIR_KEYWORD_SIZEOF},
                {U"static", KEFIR_KEYWORD_STATIC},
                {U"struct", KEFIR_KEYWORD_STRUCT},
                {U"switch", KEFIR_KEYWORD_SWITCH},
                {U"typedef", KEFIR_KEYWORD_TYPEDEF},
                {U"union", KEFIR_KEYWORD_UNION},
                {U"unsigned", KEFIR_KEYWORD_UNSIGNED},
                {U"void", KEFIR_KEYWORD_VOID},
                {U"volatile", KEFIR_KEYWORD_VOLATILE},
                {U"while", KEFIR_KEYWORD_WHILE},
                {U"_Alignas", KEFIR_KEYWORD_ALIGNAS},
                {U"_Alignof", KEFIR_KEYWORD_ALIGNOF},
                {U"_Atomic", KEFIR_KEYWORD_ATOMIC},
                {U"_Bool", KEFIR_KEYWORD_BOOL},
                {U"_Complex", KEFIR_KEYWORD_COMPLEX},
                {U"_Generic", KEFIR_KEYWORD_GENERIC},
                {U"_Imaginary", KEFIR_KEYWORD_IMAGINARY},
                {U"_Noreturn", KEFIR_KEYWORD_NORETURN},
                {U"_Static_assert", KEFIR_KEYWORD_STATIC_ASSERT},
                {U"_Thread_local", KEFIR_KEYWORD_THREAD_LOCAL}};
const kefir_size_t KEYWORDS_LENGTH = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

kefir_result_t kefir_lexer_get_keyword(const kefir_char32_t *string, kefir_keyword_token_t *keyword) {
    REQUIRE(string != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string"));
    REQUIRE(keyword != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to keyword"));

    for (kefir_size_t i = 0; i < KEYWORDS_LENGTH; i++) {
        struct KeywordEntry *entry = &KEYWORDS[i];
        kefir_int_t cmpres = kefir_strcmp32(string, entry->literal);
        REQUIRE(cmpres != KEFIR_STRCMP32_ERROR, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Error while comparing strings"));
        if (cmpres == 0) {
            *keyword = entry->keyword;
            return KEFIR_OK;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match keyword");
}
