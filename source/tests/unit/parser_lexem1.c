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

#include "kefir/parser/lexem.h"
#include "kefir/test/unit_test.h"
#include "kefir/test/util.h"

DEFINE_CASE(parser_lexem_construction_sentinel, "Parser - sentinel tokens") {
    struct kefir_token token;
    ASSERT_OK(kefir_token_new_sentinel(&token));
    ASSERT(token.klass == KEFIR_TOKEN_SENTINEL);
}
END_CASE

DEFINE_CASE(parser_lexem_construction_keyword, "Parser - keyword tokens") {
#define ASSERT_KEYWORD(_keyword)                                 \
    do {                                                         \
        struct kefir_token token;                                \
        REQUIRE_OK(kefir_token_new_keyword((_keyword), &token)); \
        ASSERT(token.klass == KEFIR_TOKEN_KEYWORD);              \
        ASSERT(token.keyword == (_keyword));                     \
        REQUIRE_OK(kefir_token_free(&kft_mem, &token));          \
    } while (0);
    ASSERT_KEYWORD(KEFIR_KEYWORD_AUTO);
    ASSERT_KEYWORD(KEFIR_KEYWORD_BREAK);
    ASSERT_KEYWORD(KEFIR_KEYWORD_CASE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_CHAR);
    ASSERT_KEYWORD(KEFIR_KEYWORD_CONST);
    ASSERT_KEYWORD(KEFIR_KEYWORD_CONTINUE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_DEFAULT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_DO);
    ASSERT_KEYWORD(KEFIR_KEYWORD_DOUBLE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_ELSE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_ENUM);
    ASSERT_KEYWORD(KEFIR_KEYWORD_EXTERN);
    ASSERT_KEYWORD(KEFIR_KEYWORD_FLOAT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_FOR);
    ASSERT_KEYWORD(KEFIR_KEYWORD_GOTO);
    ASSERT_KEYWORD(KEFIR_KEYWORD_IF);
    ASSERT_KEYWORD(KEFIR_KEYWORD_INLINE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_INT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_LONG);
    ASSERT_KEYWORD(KEFIR_KEYWORD_REGISTER);
    ASSERT_KEYWORD(KEFIR_KEYWORD_RESTRICT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_RETURN);
    ASSERT_KEYWORD(KEFIR_KEYWORD_SHORT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_SIGNED);
    ASSERT_KEYWORD(KEFIR_KEYWORD_SIZEOF);
    ASSERT_KEYWORD(KEFIR_KEYWORD_STATIC);
    ASSERT_KEYWORD(KEFIR_KEYWORD_STRUCT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_SWITCH);
    ASSERT_KEYWORD(KEFIR_KEYWORD_TYPEDEF);
    ASSERT_KEYWORD(KEFIR_KEYWORD_UNION);
    ASSERT_KEYWORD(KEFIR_KEYWORD_UNSIGNED);
    ASSERT_KEYWORD(KEFIR_KEYWORD_VOID);
    ASSERT_KEYWORD(KEFIR_KEYWORD_VOLATILE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_WHILE);
    ASSERT_KEYWORD(KEFIR_KEYWORD_ALIGNAS);
    ASSERT_KEYWORD(KEFIR_KEYWORD_ALIGNOF);
    ASSERT_KEYWORD(KEFIR_KEYWORD_ATOMIC);
    ASSERT_KEYWORD(KEFIR_KEYWORD_BOOL);
    ASSERT_KEYWORD(KEFIR_KEYWORD_COMPLEX);
    ASSERT_KEYWORD(KEFIR_KEYWORD_GENERIC);
    ASSERT_KEYWORD(KEFIR_KEYWORD_IMAGINARY);
    ASSERT_KEYWORD(KEFIR_KEYWORD_NORETURN);
    ASSERT_KEYWORD(KEFIR_KEYWORD_STATIC_ASSERT);
    ASSERT_KEYWORD(KEFIR_KEYWORD_THREAD_LOCAL);
#undef ASSERT_KEYWORD
}
END_CASE

DEFINE_CASE(parser_lexem_construction_identifier, "Parser - identifier tokens") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_token token;
    ASSERT_NOK(kefir_token_new_identifier(&kft_mem, NULL, "abc", &token));
    ASSERT_NOK(kefir_token_new_identifier(NULL, &symbols, "abc", &token));
    ASSERT_NOK(kefir_token_new_identifier(NULL, NULL, NULL, &token));
    ASSERT_NOK(kefir_token_new_identifier(NULL, NULL, "", &token));

#define ASSERT_IDENTIFIER(_id)                                                    \
    do {                                                                          \
        ASSERT_OK(kefir_token_new_identifier(&kft_mem, &symbols, (_id), &token)); \
        ASSERT(token.klass == KEFIR_TOKEN_IDENTIFIER);                            \
        ASSERT(token.identifier != NULL);                                         \
        ASSERT(strcmp(token.identifier, (_id)) == 0);                             \
        ASSERT_OK(kefir_token_free(&kft_mem, &token));                            \
    } while (0)

    ASSERT_IDENTIFIER("a");
    ASSERT_IDENTIFIER("abc");
    ASSERT_IDENTIFIER("ABC123abc");
    ASSERT_IDENTIFIER("___abc_test_156TESTtest_TEST");

#undef ASSERT_IDENTIFIER

#define ASSERT_IDENTIFIER(_id)                                            \
    do {                                                                  \
        ASSERT_OK(kefir_token_new_identifier(NULL, NULL, (_id), &token)); \
        ASSERT(token.klass == KEFIR_TOKEN_IDENTIFIER);                    \
        ASSERT(token.identifier != NULL);                                 \
        ASSERT(strcmp(token.identifier, (_id)) == 0);                     \
        ASSERT_OK(kefir_token_free(&kft_mem, &token));                    \
    } while (0)

    ASSERT_IDENTIFIER("a");
    ASSERT_IDENTIFIER("abc");
    ASSERT_IDENTIFIER("ABC123abc");
    ASSERT_IDENTIFIER("___abc_test_156TESTtest_TEST");

#undef ASSERT_IDENTIFIER

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(parser_lexem_construction_constants, "Parser - constant tokens") {
    struct kefir_token token;
#define ASSERT_CONSTANT(_fn, _type, _field, _value)    \
    do {                                               \
        ASSERT_OK(_fn((_value), &token));              \
        ASSERT(token.klass == KEFIR_TOKEN_CONSTANT);   \
        ASSERT(token.constant.type == (_type));        \
        ASSERT(token.constant._field == (_value));     \
        ASSERT_OK(kefir_token_free(&kft_mem, &token)); \
    } while (0)

    for (kefir_int64_t i = -10; i < 10; i++) {
        ASSERT_CONSTANT(kefir_token_new_constant_int, KEFIR_CONSTANT_TOKEN_INTEGER, integer, 1 + i);
        ASSERT_CONSTANT(kefir_token_new_constant_uint, KEFIR_CONSTANT_TOKEN_UNSIGNED_INTEGER, uinteger,
                        (kefir_uint64_t) 11 + i);
        ASSERT_CONSTANT(kefir_token_new_constant_long, KEFIR_CONSTANT_TOKEN_LONG_INTEGER, integer, 2 + i);
        ASSERT_CONSTANT(kefir_token_new_constant_ulong, KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_INTEGER, uinteger,
                        (kefir_uint64_t) 13 + i);
        ASSERT_CONSTANT(kefir_token_new_constant_long_long, KEFIR_CONSTANT_TOKEN_LONG_LONG_INTEGER, integer, 4 + i);
        ASSERT_CONSTANT(kefir_token_new_constant_ulong_long, KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_LONG_INTEGER, uinteger,
                        (kefir_uint64_t) 15 + i);
        ASSERT_CONSTANT(kefir_token_new_constant_char, KEFIR_CONSTANT_TOKEN_CHAR, character, 'A' + i);
        ASSERT_CONSTANT(kefir_token_new_constant_wide_char, KEFIR_CONSTANT_TOKEN_WIDE_CHAR, wide_char, U'B' + i);
        ASSERT_CONSTANT(kefir_token_new_constant_unicode16_char, KEFIR_CONSTANT_TOKEN_UNICODE16_CHAR, unicode16_char,
                        U'C' + i);
        ASSERT_CONSTANT(kefir_token_new_constant_unicode32_char, KEFIR_CONSTANT_TOKEN_UNICODE32_CHAR, unicode32_char,
                        U'D' + i);

        ASSERT_OK(kefir_token_new_constant_float(((kefir_float32_t) i) / 10, &token));
        ASSERT(token.klass == KEFIR_TOKEN_CONSTANT);
        ASSERT(token.constant.type == KEFIR_CONSTANT_TOKEN_FLOAT);
        ASSERT(FLOAT_EQUALS(token.constant.float32, ((kefir_float32_t) i) / 10, FLOAT_EPSILON));
        ASSERT_OK(kefir_token_free(&kft_mem, &token));

        ASSERT_OK(kefir_token_new_constant_double(((kefir_float64_t) i) / 10, &token));
        ASSERT(token.klass == KEFIR_TOKEN_CONSTANT);
        ASSERT(token.constant.type == KEFIR_CONSTANT_TOKEN_DOUBLE);
        ASSERT(DOUBLE_EQUALS(token.constant.float64, ((kefir_float64_t) i) / 10, DOUBLE_EPSILON));
        ASSERT_OK(kefir_token_free(&kft_mem, &token));
    }

#undef ASSERT_CONSTANT
}
END_CASE

DEFINE_CASE(parser_lexem_construction_string_literals, "Parser - string literal tokens") {
    struct kefir_token token;
#define ASSERT_STRING_LITERAL(_literal, _length)                                            \
    do {                                                                                    \
        ASSERT_OK(kefir_token_new_string_literal(&kft_mem, (_literal), (_length), &token)); \
        ASSERT(token.klass == KEFIR_TOKEN_STRING_LITERAL);                                  \
        ASSERT(token.string_literal.content != NULL);                                       \
        ASSERT(token.string_literal.content != (_literal));                                 \
        ASSERT(token.string_literal.length == (_length));                                   \
        ASSERT(memcmp(token.string_literal.content, (_literal), (_length)) == 0);           \
        ASSERT_OK(kefir_token_free(&kft_mem, &token));                                      \
    } while (0)

    const char *MSG[] = {"", "abc", "test test test", "One two three\n\n\n\t    Test...test...test...123",
                         "Hello, cruel-cruel-cruel world!"};
    kefir_size_t MSG_LEN = sizeof(MSG) / sizeof(MSG[0]);
    for (kefir_size_t i = 0; i < MSG_LEN; i++) {
        ASSERT_STRING_LITERAL(MSG[i], strlen(MSG[i]));
    }

    const char line[] = " \0\0\0\n\t\0";
    ASSERT_STRING_LITERAL(line, sizeof(line));

#undef ASSERT_STRING_LITERAL
}
END_CASE

DEFINE_CASE(parser_lexem_construction_punctuator, "Parser - punctuator tokens") {
#define ASSERT_PUNCTUATOR(_punctuator)                                 \
    do {                                                               \
        struct kefir_token token;                                      \
        REQUIRE_OK(kefir_token_new_punctuator((_punctuator), &token)); \
        ASSERT(token.klass == KEFIR_TOKEN_PUNCTUATOR);                 \
        ASSERT(token.punctuator == (_punctuator));                     \
        REQUIRE_OK(kefir_token_free(&kft_mem, &token));                \
    } while (0);

    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_LEFT_BRACKET);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_RIGHT_BRACKET);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_LEFT_PARENTHESE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_LEFT_BRACE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_RIGHT_BRACE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DOT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_RIGHT_ARROW);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DOUBLE_PLUS);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DOUBLE_MINUS);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_AMPERSAND);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_STAR);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_MINUS);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_TILDE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_EXCLAMATION_MARK);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_SLASH);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_PERCENT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_LEFT_SHIFT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_RIGHT_SHIFT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_LESS_THAN);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_GREATER_THAN);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_LESS_OR_EQUAL);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_GREATER_OR_EQUAL);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_EQUAL);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_NOT_EQUAL);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_CARET);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_VBAR);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DOUBLE_AMPERSAND);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DOUBLE_VBAR);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_QUESTION_MARK);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_COLON);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_SEMICOLON);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ELLIPSIS);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_MULTIPLY);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_DIVIDE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_MODULO);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_ADD);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_SUBTRACT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_AND);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_XOR);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_ASSIGN_OR);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_COMMA);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_HASH);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DOUBLE_HASH);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACKET);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACKET);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACE);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DIGRAPH_HASH);
    ASSERT_PUNCTUATOR(KEFIR_PUNCTUATOR_DIGRAPH_DOUBLE_HASH);

#undef ASSERT_PUNCTUATOR
}
END_CASE

DEFINE_CASE(parser_lexem_move, "Parser - moving tokens") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_token src, dst;
    ASSERT_NOK(kefir_token_move(NULL, NULL));
    ASSERT_NOK(kefir_token_move(&dst, NULL));
    ASSERT_NOK(kefir_token_move(NULL, &src));

    ASSERT_OK(kefir_token_new_sentinel(&src));
    ASSERT_OK(kefir_token_move(&dst, &src));
    ASSERT(dst.klass == KEFIR_TOKEN_SENTINEL);
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_token_new_keyword(KEFIR_KEYWORD_DO, &src));
    ASSERT_OK(kefir_token_move(&dst, &src));
    ASSERT(dst.klass == KEFIR_TOKEN_KEYWORD);
    ASSERT(dst.keyword == KEFIR_KEYWORD_DO);
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_token_new_identifier(&kft_mem, &symbols, "testTEST", &src));
    ASSERT_OK(kefir_token_move(&dst, &src));
    ASSERT(dst.klass == KEFIR_TOKEN_IDENTIFIER);
    ASSERT(strcmp(dst.identifier, "testTEST") == 0);
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_token_new_constant_double(7.5926, &src));
    ASSERT_OK(kefir_token_move(&dst, &src));
    ASSERT(dst.klass == KEFIR_TOKEN_CONSTANT);
    ASSERT(dst.constant.type == KEFIR_CONSTANT_TOKEN_DOUBLE);
    ASSERT(DOUBLE_EQUALS(dst.constant.float64, 7.5926, DOUBLE_EPSILON));
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    const char MSG[] = "\0\0\0TEST...TEST...TEST...HELLO!!!!\0";
    ASSERT_OK(kefir_token_new_string_literal(&kft_mem, MSG, sizeof(MSG), &src));
    ASSERT_OK(kefir_token_move(&dst, &src));
    ASSERT(dst.klass == KEFIR_TOKEN_STRING_LITERAL);
    ASSERT(dst.string_literal.length == sizeof(MSG));
    ASSERT(memcmp(MSG, dst.string_literal.content, sizeof(MSG)) == 0);
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(parser_lexem_copy, "Parser - copying tokens") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_token src, dst;
    ASSERT_NOK(kefir_token_copy(&kft_mem, NULL, NULL));
    ASSERT_NOK(kefir_token_copy(&kft_mem, &dst, NULL));
    ASSERT_NOK(kefir_token_copy(&kft_mem, NULL, &src));
    ASSERT_NOK(kefir_token_copy(NULL, &dst, &src));

    ASSERT_OK(kefir_token_new_sentinel(&src));
    ASSERT_OK(kefir_token_copy(&kft_mem, &dst, &src));
    ASSERT(src.klass == KEFIR_TOKEN_SENTINEL);
    ASSERT(dst.klass == KEFIR_TOKEN_SENTINEL);
    ASSERT_OK(kefir_token_free(&kft_mem, &src));
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_token_new_keyword(KEFIR_KEYWORD_DO, &src));
    ASSERT_OK(kefir_token_copy(&kft_mem, &dst, &src));
    ASSERT(src.klass == KEFIR_TOKEN_KEYWORD);
    ASSERT(src.keyword == KEFIR_KEYWORD_DO);
    ASSERT(dst.klass == KEFIR_TOKEN_KEYWORD);
    ASSERT(dst.keyword == KEFIR_KEYWORD_DO);
    ASSERT_OK(kefir_token_free(&kft_mem, &src));
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_token_new_identifier(&kft_mem, &symbols, "testTEST", &src));
    ASSERT_OK(kefir_token_copy(&kft_mem, &dst, &src));
    ASSERT(src.klass == KEFIR_TOKEN_IDENTIFIER);
    ASSERT(strcmp(src.identifier, "testTEST") == 0);
    ASSERT(dst.klass == KEFIR_TOKEN_IDENTIFIER);
    ASSERT(strcmp(dst.identifier, "testTEST") == 0);
    ASSERT_OK(kefir_token_free(&kft_mem, &src));
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_token_new_constant_double(7.5926, &src));
    ASSERT_OK(kefir_token_copy(&kft_mem, &dst, &src));
    ASSERT(src.klass == KEFIR_TOKEN_CONSTANT);
    ASSERT(src.constant.type == KEFIR_CONSTANT_TOKEN_DOUBLE);
    ASSERT(DOUBLE_EQUALS(src.constant.float64, 7.5926, DOUBLE_EPSILON));
    ASSERT(dst.klass == KEFIR_TOKEN_CONSTANT);
    ASSERT(dst.constant.type == KEFIR_CONSTANT_TOKEN_DOUBLE);
    ASSERT(DOUBLE_EQUALS(dst.constant.float64, 7.5926, DOUBLE_EPSILON));
    ASSERT_OK(kefir_token_free(&kft_mem, &src));
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    const char MSG[] = "\0\0\0TEST...TEST...TEST...HELLO!!!!\0";
    ASSERT_OK(kefir_token_new_string_literal(&kft_mem, MSG, sizeof(MSG), &src));
    ASSERT_OK(kefir_token_copy(&kft_mem, &dst, &src));
    ASSERT(src.klass == KEFIR_TOKEN_STRING_LITERAL);
    ASSERT(src.string_literal.length == sizeof(MSG));
    ASSERT(memcmp(MSG, src.string_literal.content, sizeof(MSG)) == 0);
    ASSERT(src.string_literal.content != MSG);
    ASSERT(dst.klass == KEFIR_TOKEN_STRING_LITERAL);
    ASSERT(dst.string_literal.length == sizeof(MSG));
    ASSERT(memcmp(MSG, dst.string_literal.content, sizeof(MSG)) == 0);
    ASSERT(dst.string_literal.content != MSG);
    ASSERT_OK(kefir_token_free(&kft_mem, &src));
    ASSERT_OK(kefir_token_free(&kft_mem, &dst));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
