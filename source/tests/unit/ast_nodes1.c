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

#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_nodes_constants, "AST nodes - constants") {
    struct kefir_ast_constant *bool1 = kefir_ast_new_constant_bool(&kft_mem, true);
    struct kefir_ast_constant *bool2 = kefir_ast_new_constant_bool(&kft_mem, false);
    ASSERT(bool1 != NULL);
    ASSERT(bool2 != NULL);
    ASSERT(bool1->base.klass->type == KEFIR_AST_CONSTANT);
    ASSERT(bool2->base.klass->type == KEFIR_AST_CONSTANT);
    ASSERT(bool1->base.self == bool1);
    ASSERT(bool2->base.self == bool2);
    ASSERT(bool1->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(bool2->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(bool1->value.boolean);
    ASSERT(!bool2->value.boolean);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(bool1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(bool2)));

    for (kefir_char_t chr = KEFIR_CHAR_MIN; chr < KEFIR_CHAR_MAX; chr++) {
        struct kefir_ast_constant *chrc = kefir_ast_new_constant_char(&kft_mem, chr);
        ASSERT(chrc != NULL);
        ASSERT(chrc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(chrc->base.self = chrc);
        ASSERT(chrc->type = KEFIR_AST_CHAR_CONSTANT);
        ASSERT(chrc->value.character == chr);
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(chrc)));
    }

    for (kefir_char32_t chr = KEFIR_CHAR32_MIN; chr < 4096; chr++) {
        struct kefir_ast_constant *wchrc = kefir_ast_new_constant_wide_char(&kft_mem, chr);
        struct kefir_ast_constant *u16chrc = kefir_ast_new_constant_unicode16_char(&kft_mem, chr);
        struct kefir_ast_constant *u32chrc = kefir_ast_new_constant_unicode32_char(&kft_mem, chr);
        ASSERT(wchrc != NULL);
        ASSERT(u16chrc != NULL);
        ASSERT(u32chrc != NULL);
        ASSERT(wchrc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(u16chrc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(u32chrc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(wchrc->base.self = wchrc);
        ASSERT(u16chrc->base.self = u16chrc);
        ASSERT(u32chrc->base.self = u32chrc);
        ASSERT(wchrc->type = KEFIR_AST_WIDE_CHAR_CONSTANT);
        ASSERT(u16chrc->type = KEFIR_AST_UNICODE16_CHAR_CONSTANT);
        ASSERT(u32chrc->type = KEFIR_AST_UNICODE32_CHAR_CONSTANT);
        ASSERT(wchrc->value.wide_character == (kefir_wchar_t) chr);
        ASSERT(u16chrc->value.unicode16_character == chr);
        ASSERT(u32chrc->value.unicode32_character == chr);
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(wchrc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(u16chrc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(u32chrc)));
    }

    for (kefir_int_t i = -1000; i < 1000; i++) {
        struct kefir_ast_constant *intc = kefir_ast_new_constant_int(&kft_mem, i);
        struct kefir_ast_constant *longc = kefir_ast_new_constant_long(&kft_mem, i);
        struct kefir_ast_constant *llongc = kefir_ast_new_constant_long_long(&kft_mem, i);
        ASSERT(intc != NULL);
        ASSERT(longc != NULL);
        ASSERT(llongc != NULL);
        ASSERT(intc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(longc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(llongc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(intc->base.self == intc);
        ASSERT(longc->base.self == longc);
        ASSERT(llongc->base.self == llongc);
        ASSERT(intc->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(longc->type == KEFIR_AST_LONG_CONSTANT);
        ASSERT(llongc->type == KEFIR_AST_LONG_LONG_CONSTANT);
        ASSERT(intc->value.integer == i);
        ASSERT(longc->value.long_integer == i);
        ASSERT(llongc->value.long_long == i);
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(intc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(longc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(llongc)));
    }

    for (kefir_uint_t i = 0; i < 2000; i++) {
        struct kefir_ast_constant *intc = kefir_ast_new_constant_uint(&kft_mem, i);
        struct kefir_ast_constant *longc = kefir_ast_new_constant_ulong(&kft_mem, i);
        struct kefir_ast_constant *llongc = kefir_ast_new_constant_ulong_long(&kft_mem, i);
        ASSERT(intc != NULL);
        ASSERT(longc != NULL);
        ASSERT(llongc != NULL);
        ASSERT(intc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(longc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(llongc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(intc->base.self == intc);
        ASSERT(longc->base.self == longc);
        ASSERT(llongc->base.self == llongc);
        ASSERT(intc->type == KEFIR_AST_UINT_CONSTANT);
        ASSERT(longc->type == KEFIR_AST_ULONG_CONSTANT);
        ASSERT(llongc->type == KEFIR_AST_ULONG_LONG_CONSTANT);
        ASSERT(intc->value.uinteger == i);
        ASSERT(longc->value.ulong_integer == i);
        ASSERT(llongc->value.ulong_long == i);
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(intc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(longc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(llongc)));
    }

    for (kefir_float32_t i = -1000.0f; i < 1000.0f; i += 0.05f) {
        struct kefir_ast_constant *floatc = kefir_ast_new_constant_float(&kft_mem, i);
        struct kefir_ast_constant *doublec = kefir_ast_new_constant_double(&kft_mem, (kefir_float64_t) i);
        ASSERT(floatc != NULL);
        ASSERT(doublec != NULL);
        ASSERT(floatc->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(doublec->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(floatc->base.self == floatc);
        ASSERT(doublec->base.self == doublec);
        ASSERT(floatc->type == KEFIR_AST_FLOAT_CONSTANT);
        ASSERT(doublec->type == KEFIR_AST_DOUBLE_CONSTANT);
        ASSERT(FLOAT_EQUALS(floatc->value.float32, i, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(doublec->value.float64, (kefir_float64_t) i, DOUBLE_EPSILON));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(floatc)));
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(doublec)));
    }
}
END_CASE

DEFINE_CASE(ast_nodes_identifiers, "AST nodes - identifiers") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_identifier *sym1 = kefir_ast_new_identifier(&kft_mem, &symbols, "symbol1");
    struct kefir_ast_identifier *sym2 = kefir_ast_new_identifier(&kft_mem, &symbols, "symbol_test1234");
    ASSERT(sym1 != NULL);
    ASSERT(sym2 != NULL);
    ASSERT(sym1->base.klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(sym2->base.klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(sym1->base.self == sym1);
    ASSERT(sym2->base.self == sym2);
    ASSERT(strcmp(sym1->identifier, "symbol1") == 0);
    ASSERT(strcmp(sym2->identifier, "symbol_test1234") == 0);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym2)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_string_literals_multibyte, "AST nodes - multibyte string literals") {
    struct kefir_ast_string_literal *sym1 = KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!");
    struct kefir_ast_string_literal *sym2 = KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Goodbue, cruel world!");
    ASSERT(sym1 != NULL);
    ASSERT(sym2 != NULL);
    ASSERT(sym1->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym2->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym1->base.self == sym1);
    ASSERT(sym2->base.self == sym2);
    ASSERT(sym1->type == KEFIR_AST_STRING_LITERAL_MULTIBYTE);
    ASSERT(sym2->type == KEFIR_AST_STRING_LITERAL_MULTIBYTE);
    ASSERT(strcmp(sym1->literal, "Hello, world!") == 0);
    ASSERT(strcmp(sym2->literal, "Goodbue, cruel world!") == 0);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym2)));
}
END_CASE

DEFINE_CASE(ast_nodes_string_literals_unicode8, "AST nodes - unicode8 string literals") {
    const char STRING1[] = u8"Hello, pretty world!\0";
    const char STRING2[] = u8"Goodbye,\0 not so pretty world!\n";

    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_string_literal *sym1 = kefir_ast_new_string_literal_unicode8(&kft_mem, STRING1, sizeof(STRING1));
    struct kefir_ast_string_literal *sym2 = kefir_ast_new_string_literal_unicode8(&kft_mem, STRING2, sizeof(STRING2));
    ASSERT(sym1 != NULL);
    ASSERT(sym2 != NULL);
    ASSERT(sym1->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym2->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym1->base.self == sym1);
    ASSERT(sym2->base.self == sym2);
    ASSERT(sym1->type == KEFIR_AST_STRING_LITERAL_UNICODE8);
    ASSERT(sym2->type == KEFIR_AST_STRING_LITERAL_UNICODE8);
    ASSERT(memcmp(sym1->literal, STRING1, sizeof(STRING1)) == 0);
    ASSERT(memcmp(sym2->literal, STRING2, sizeof(STRING2)) == 0);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym2)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_string_literals_unicode16, "AST nodes - unicode16 string literals") {
    const kefir_char16_t STRING1[] = u"Another char16 string constant here\r\n...";
    const kefir_char16_t STRING2[] = u"Yet another\0\00one. Surprise\u0180";

    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_string_literal *sym1 = kefir_ast_new_string_literal_unicode16(&kft_mem, STRING1, sizeof(STRING1));
    struct kefir_ast_string_literal *sym2 = kefir_ast_new_string_literal_unicode16(&kft_mem, STRING2, sizeof(STRING2));
    ASSERT(sym1 != NULL);
    ASSERT(sym2 != NULL);
    ASSERT(sym1->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym2->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym1->base.self == sym1);
    ASSERT(sym2->base.self == sym2);
    ASSERT(sym1->type == KEFIR_AST_STRING_LITERAL_UNICODE16);
    ASSERT(sym2->type == KEFIR_AST_STRING_LITERAL_UNICODE16);
    ASSERT(memcmp(sym1->literal, STRING1, sizeof(STRING1)) == 0);
    ASSERT(memcmp(sym2->literal, STRING2, sizeof(STRING2)) == 0);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym2)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_string_literals_unicode32, "AST nodes - unicode32 string literals") {
    const kefir_char32_t STRING1[] = U"\t\v\fThe real text actually goes there\n!\0";
    const kefir_char32_t STRING2[] = U"\0Joking. In fact it\'s here\n\0Is it\?\"";

    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_string_literal *sym1 = kefir_ast_new_string_literal_unicode32(&kft_mem, STRING1, sizeof(STRING1));
    struct kefir_ast_string_literal *sym2 = kefir_ast_new_string_literal_unicode32(&kft_mem, STRING2, sizeof(STRING2));
    ASSERT(sym1 != NULL);
    ASSERT(sym2 != NULL);
    ASSERT(sym1->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym2->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym1->base.self == sym1);
    ASSERT(sym2->base.self == sym2);
    ASSERT(sym1->type == KEFIR_AST_STRING_LITERAL_UNICODE32);
    ASSERT(sym2->type == KEFIR_AST_STRING_LITERAL_UNICODE32);
    ASSERT(memcmp(sym1->literal, STRING1, sizeof(STRING1)) == 0);
    ASSERT(memcmp(sym2->literal, STRING2, sizeof(STRING2)) == 0);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym2)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_string_literals_wide, "AST nodes - wide string literals") {
    const kefir_wchar_t STRING1[] = L"Some fake wide char...\n\tRight here\U00000100";
    const kefir_wchar_t STRING2[] = L"And\000here\x00000 as well.\vSurprising\\";

    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_string_literal *sym1 = kefir_ast_new_string_literal_wide(&kft_mem, STRING1, sizeof(STRING1));
    struct kefir_ast_string_literal *sym2 = kefir_ast_new_string_literal_wide(&kft_mem, STRING2, sizeof(STRING2));
    ASSERT(sym1 != NULL);
    ASSERT(sym2 != NULL);
    ASSERT(sym1->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym2->base.klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(sym1->base.self == sym1);
    ASSERT(sym2->base.self == sym2);
    ASSERT(sym1->type == KEFIR_AST_STRING_LITERAL_WIDE);
    ASSERT(sym2->type == KEFIR_AST_STRING_LITERAL_WIDE);
    ASSERT(memcmp(sym1->literal, STRING1, sizeof(STRING1)) == 0);
    ASSERT(memcmp(sym2->literal, STRING2, sizeof(STRING2)) == 0);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(sym2)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_type_name, "AST nodes - type name") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_type_name *type1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_type_name *type2 = kefir_ast_new_type_name(
        &kft_mem,
        kefir_ast_declarator_pointer(
            &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 256)),
                                                 kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));

    ASSERT(type1 != NULL);
    ASSERT(type2 != NULL);
    ASSERT(type1->base.klass->type == KEFIR_AST_TYPE_NAME);
    ASSERT(type2->base.klass->type == KEFIR_AST_TYPE_NAME);
    ASSERT(type1->base.self == type1);
    ASSERT(type2->base.self == type2);

    struct kefir_ast_declarator_specifier *specifier1 = NULL;
    struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(&type1->type_decl.specifiers, &specifier1);
    ASSERT(iter != NULL);
    ASSERT(specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_VOID);
    ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter, &specifier1));
    ASSERT(iter == NULL);
    ASSERT(type1->type_decl.declarator != NULL);
    ASSERT(type1->type_decl.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(kefir_ast_type_qualifier_list_iter(&type1->type_decl.declarator->pointer.type_qualifiers, NULL) == NULL);
    ASSERT(type1->type_decl.declarator->pointer.declarator != NULL);
    ASSERT(type1->type_decl.declarator->pointer.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(type1->type_decl.declarator->pointer.declarator->identifier == NULL);

    iter = kefir_ast_declarator_specifier_list_iter(&type2->type_decl.specifiers, &specifier1);
    ASSERT(iter != NULL);
    ASSERT(specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_CHAR);
    ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter, &specifier1));
    ASSERT(iter == NULL);
    ASSERT(type2->type_decl.declarator != NULL);
    ASSERT(type2->type_decl.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(kefir_ast_type_qualifier_list_iter(&type2->type_decl.declarator->pointer.type_qualifiers, NULL) == NULL);
    ASSERT(type2->type_decl.declarator->pointer.declarator != NULL);
    ASSERT(type2->type_decl.declarator->pointer.declarator->klass == KEFIR_AST_DECLARATOR_ARRAY);
    ASSERT(type2->type_decl.declarator->pointer.declarator->array.type == KEFIR_AST_DECLARATOR_ARRAY_BOUNDED);
    ASSERT(!type2->type_decl.declarator->pointer.declarator->array.static_array);
    ASSERT(kefir_ast_type_qualifier_list_iter(&type2->type_decl.declarator->pointer.declarator->array.type_qualifiers,
                                              NULL) == NULL);
    ASSERT(type2->type_decl.declarator->pointer.declarator->array.length->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(type2->type_decl.declarator->pointer.declarator->array.declarator != NULL);
    ASSERT(type2->type_decl.declarator->pointer.declarator->array.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(type2->type_decl.declarator->pointer.declarator->array.declarator->identifier == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_array_subscripts, "AST nodes - array sybscripts") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_identifier *array = kefir_ast_new_identifier(&kft_mem, &symbols, "array1");
    ASSERT(array != NULL);
    struct kefir_ast_constant *index = kefir_ast_new_constant_int(&kft_mem, 100);
    ASSERT(index != NULL);
    struct kefir_ast_array_subscript *subscript =
        kefir_ast_new_array_subscript(&kft_mem, KEFIR_AST_NODE_BASE(array), KEFIR_AST_NODE_BASE(index));
    ASSERT(subscript != NULL);
    ASSERT(subscript->base.klass->type == KEFIR_AST_ARRAY_SUBSCRIPT);
    ASSERT(subscript->base.self == subscript);
    ASSERT(subscript->array == KEFIR_AST_NODE_BASE(array));
    ASSERT(subscript->subscript == KEFIR_AST_NODE_BASE(index));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(subscript)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_function_calls, "AST nodes - function calls") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_identifier *function = kefir_ast_new_identifier(&kft_mem, &symbols, "function1");
    struct kefir_ast_function_call *call = kefir_ast_new_function_call(&kft_mem, KEFIR_AST_NODE_BASE(function));
    ASSERT_OK(kefir_ast_function_call_append(
        &kft_mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "variable1"))));
    ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))));
    ASSERT_OK(kefir_ast_function_call_append(
        &kft_mem, call,
        KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "array1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 5))))));
    ASSERT(call->function->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(kefir_list_length(&call->arguments) == 3);
    ASSERT(((struct kefir_ast_node_base *) kefir_list_at(&call->arguments, 0)->value)->klass->type ==
           KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_node_base *) kefir_list_at(&call->arguments, 1)->value)->klass->type ==
           KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_node_base *) kefir_list_at(&call->arguments, 2)->value)->klass->type ==
           KEFIR_AST_ARRAY_SUBSCRIPT);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(call)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_builtins, "AST nodes - builtins") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_builtin *builtin = kefir_ast_new_builtin(&kft_mem, KEFIR_AST_BUILTIN_VA_ARG);
    ASSERT_OK(kefir_ast_builtin_append(&kft_mem, builtin,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "list"))));
    ASSERT_OK(
        kefir_ast_builtin_append(&kft_mem, builtin, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))));
    ASSERT_OK(kefir_ast_builtin_append(
        &kft_mem, builtin,
        KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "array1")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 5))))));
    ASSERT(builtin->builtin == KEFIR_AST_BUILTIN_VA_ARG);
    ASSERT(kefir_list_length(&builtin->arguments) == 3);
    ASSERT(((struct kefir_ast_node_base *) kefir_list_at(&builtin->arguments, 0)->value)->klass->type ==
           KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_node_base *) kefir_list_at(&builtin->arguments, 1)->value)->klass->type ==
           KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_node_base *) kefir_list_at(&builtin->arguments, 2)->value)->klass->type ==
           KEFIR_AST_ARRAY_SUBSCRIPT);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(builtin)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_struct_members, "AST nodes - struct members") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    struct kefir_ast_struct_member *member = kefir_ast_new_struct_member(
        &kft_mem, &symbols,
        KEFIR_AST_NODE_BASE(kefir_ast_new_struct_indirect_member(
            &kft_mem, &symbols,
            KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "array1")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 13456)))),
            "field1")),
        "field2");
    ASSERT(member != NULL);
    ASSERT(member->base.klass->type == KEFIR_AST_STRUCTURE_MEMBER);
    ASSERT(member->base.self == member);
    ASSERT(strcmp(member->member, "field2") == 0);
    ASSERT(member->structure != NULL);
    ASSERT(((struct kefir_ast_node_base *) member->structure)->klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER);
    ASSIGN_DECL_CAST(struct kefir_ast_struct_member *, member2,
                     ((struct kefir_ast_node_base *) member->structure)->self);
    ASSERT(member2->base.klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER);
    ASSERT(strcmp(member2->member, "field1") == 0);
    ASSERT(member2->structure != NULL);
    ASSERT(((struct kefir_ast_node_base *) member2->structure)->klass->type == KEFIR_AST_ARRAY_SUBSCRIPT);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(member)));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_unary_operations, "AST nodes - unary operations") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    kefir_ast_unary_operation_type_t UNARY_OPERATORS[] = {KEFIR_AST_OPERATION_PLUS, KEFIR_AST_OPERATION_NEGATE,
                                                          KEFIR_AST_OPERATION_INVERT,
                                                          KEFIR_AST_OPERATION_LOGICAL_NEGATE};
    const kefir_size_t UNARY_OPERATOR_LENGTH = sizeof(UNARY_OPERATORS) / sizeof(UNARY_OPERATORS[0]);
    for (kefir_size_t i = 0; i < UNARY_OPERATOR_LENGTH; i++) {
        struct kefir_ast_unary_operation *unary_op = kefir_ast_new_unary_operation(
            &kft_mem, UNARY_OPERATORS[i], KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, i)));
        ASSERT(unary_op != NULL);
        ASSERT(unary_op->base.klass->type == KEFIR_AST_UNARY_OPERATION);
        ASSERT(unary_op->base.self == unary_op);
        ASSERT(unary_op->type == UNARY_OPERATORS[i]);
        ASSERT(unary_op->arg != NULL);
        ASSERT(unary_op->arg->klass->type == KEFIR_AST_CONSTANT);
        ASSIGN_DECL_CAST(struct kefir_ast_constant *, arg, unary_op->arg);
        ASSERT(arg->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(arg->value.uinteger == i);
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(unary_op)));
    }
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_binary_operations, "AST nodes - binary operations") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    kefir_ast_binary_operation_type_t BINARY_OPERATORS[] = {
        KEFIR_AST_OPERATION_ADD,         KEFIR_AST_OPERATION_SUBTRACT,     KEFIR_AST_OPERATION_MULTIPLY,
        KEFIR_AST_OPERATION_DIVIDE,      KEFIR_AST_OPERATION_MODULO,       KEFIR_AST_OPERATION_SHIFT_LEFT,
        KEFIR_AST_OPERATION_SHIFT_RIGHT, KEFIR_AST_OPERATION_LESS,         KEFIR_AST_OPERATION_LESS_EQUAL,
        KEFIR_AST_OPERATION_GREATER,     KEFIR_AST_OPERATION_GREATER_EQUAL};
    const kefir_size_t BINARY_OPERATOR_LENGTH = sizeof(BINARY_OPERATORS) / sizeof(BINARY_OPERATORS[0]);

    for (kefir_size_t i = 0; i < BINARY_OPERATOR_LENGTH; i++) {
        struct kefir_ast_binary_operation *binary_op = kefir_ast_new_binary_operation(
            &kft_mem, BINARY_OPERATORS[i],
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "helloWorld")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, i)));
        ASSERT(binary_op != NULL);
        ASSERT(binary_op->base.klass->type == KEFIR_AST_BINARY_OPERATION);
        ASSERT(binary_op->base.self == binary_op);
        ASSERT(binary_op->type == BINARY_OPERATORS[i]);
        ASSERT(binary_op->arg1 != NULL);
        ASSERT(binary_op->arg1->klass->type == KEFIR_AST_IDENTIFIER);
        ASSIGN_DECL_CAST(struct kefir_ast_identifier *, arg1, binary_op->arg1);
        ASSERT(arg1->base.klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(strcmp(arg1->identifier, "helloWorld") == 0);
        ASSERT(binary_op->arg2 != NULL);
        ASSERT(binary_op->arg2->klass->type == KEFIR_AST_CONSTANT);
        ASSIGN_DECL_CAST(struct kefir_ast_constant *, arg2, binary_op->arg2);
        ASSERT(arg2->base.klass->type == KEFIR_AST_CONSTANT);
        ASSERT(arg2->value.uinteger == i);
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(binary_op)));
    }
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
