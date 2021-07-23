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

#ifndef DECLARATOR_ANALYSIS_H_
#define DECLARATOR_ANALYSIS_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ast/declarator_specifier.h"

kefir_result_t append_specifiers(struct kefir_mem *, struct kefir_ast_declarator_specifier_list *, int, ...);

#define ASSERT_NODECL_TYPE(_mem, _context, _type, _spec_count, ...)                                               \
    do {                                                                                                          \
        struct kefir_ast_declarator_specifier_list specifiers;                                                    \
        ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));                                         \
        ASSERT_OK(append_specifiers((_mem), &specifiers, (_spec_count), __VA_ARGS__));                            \
                                                                                                                  \
        const struct kefir_ast_type *type = NULL;                                                                 \
        ASSERT_OK(                                                                                                \
            kefir_ast_analyze_declaration((_mem), (_context), &specifiers, NULL, NULL, &type, NULL, NULL, NULL)); \
                                                                                                                  \
        ASSERT(type != NULL);                                                                                     \
        ASSERT(KEFIR_AST_TYPE_SAME(type, (_type)));                                                               \
                                                                                                                  \
        ASSERT_OK(kefir_ast_declarator_specifier_list_free((_mem), &specifiers));                                 \
    } while (0)

#define ASSERT_IDENTIFIER_TYPE(_mem, _context, _type, _storage_class, _function_spec, _alignment, _spec_count, ...)    \
    do {                                                                                                               \
        struct kefir_ast_declarator_specifier_list specifiers;                                                         \
        ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));                                              \
        ASSERT_OK(append_specifiers((_mem), &specifiers, (_spec_count), __VA_ARGS__));                                 \
                                                                                                                       \
        struct kefir_ast_declarator *declarator = kefir_ast_declarator_identifier((_mem), (_context)->symbols, "var"); \
                                                                                                                       \
        const struct kefir_ast_type *type = NULL;                                                                      \
        kefir_ast_scoped_identifier_storage_t storage;                                                                 \
        kefir_ast_function_specifier_t function_specifier;                                                             \
        kefir_size_t alignment = 0;                                                                                    \
        const char *identifier = NULL;                                                                                 \
        ASSERT_OK(kefir_ast_analyze_declaration((_mem), (_context), &specifiers, declarator, &identifier, &type,       \
                                                &storage, &function_specifier, &alignment));                           \
        ASSERT_OK(kefir_ast_analyze_type((_mem), (_context), KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type));                  \
                                                                                                                       \
        ASSERT(strcmp(identifier, "var") == 0);                                                                        \
        ASSERT(type != NULL);                                                                                          \
        ASSERT(KEFIR_AST_TYPE_SAME(type, (_type)));                                                                    \
        ASSERT(storage == (_storage_class));                                                                           \
        ASSERT(function_specifier == (_function_spec));                                                                \
        ASSERT(alignment == (_alignment));                                                                             \
                                                                                                                       \
        ASSERT_OK(kefir_ast_declarator_free((_mem), declarator));                                                      \
        ASSERT_OK(kefir_ast_declarator_specifier_list_free((_mem), &specifiers));                                      \
    } while (0)

#define ASSERT_FUNCTION_TYPE(_mem, _context, _type, _storage_class, _function_spec, _spec_count, ...)               \
    do {                                                                                                            \
        struct kefir_ast_declarator_specifier_list specifiers;                                                      \
        ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));                                           \
        ASSERT_OK(append_specifiers((_mem), &specifiers, (_spec_count), __VA_ARGS__));                              \
                                                                                                                    \
        struct kefir_ast_declarator *declarator = kefir_ast_declarator_function(                                    \
            (_mem), kefir_ast_declarator_identifier((_mem), (_context)->symbols, "func"));                          \
                                                                                                                    \
        const struct kefir_ast_type *type = NULL;                                                                   \
        kefir_ast_scoped_identifier_storage_t storage;                                                              \
        kefir_ast_function_specifier_t function_specifier;                                                          \
        ASSERT_OK(kefir_ast_analyze_declaration((_mem), (_context), &specifiers, declarator, NULL, &type, &storage, \
                                                &function_specifier, NULL));                                        \
                                                                                                                    \
        ASSERT(type != NULL);                                                                                       \
        ASSERT(KEFIR_AST_TYPE_SAME(type, (_type)));                                                                 \
        ASSERT(storage == (_storage_class));                                                                        \
        ASSERT(function_specifier == (_function_spec));                                                             \
                                                                                                                    \
        ASSERT_OK(kefir_ast_declarator_free((_mem), declarator));                                                   \
        ASSERT_OK(kefir_ast_declarator_specifier_list_free((_mem), &specifiers));                                   \
    } while (0)

#endif
