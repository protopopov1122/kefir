/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#include "kefir/test/unit_test.h"
#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include <string.h>

DEFINE_CASE(ast_enum_declaration1, "AST Declarations - enum declaration #1") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_enum_specifier *specifier = kefir_ast_enum_specifier_init(&kft_mem, &symbols, "test123", false);
    ASSERT(specifier != NULL);

    ASSERT(specifier->identifier != NULL);
    ASSERT(strcmp(specifier->identifier, "test123") == 0);
    ASSERT(!specifier->complete);

    ASSERT_NOK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, NULL, NULL));
    ASSERT_NOK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "A", NULL));

    struct kefir_ast_node_base *value1 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1));
    ASSERT_NOK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "A", value1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, value1));

    ASSERT_OK(kefir_ast_enum_specifier_free(&kft_mem, specifier));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_enum_declaration2, "AST Declarations - enum declaration #2") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_enum_specifier *specifier = kefir_ast_enum_specifier_init(&kft_mem, &symbols, "some_enum", true);
    ASSERT(specifier != NULL);

    ASSERT(specifier->identifier != NULL);
    ASSERT(strcmp(specifier->identifier, "some_enum") == 0);
    ASSERT(specifier->complete);

    ASSERT_NOK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, NULL, NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "field1", NULL));

    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "field2",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "field3",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "field4", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier, &symbols, "field5",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 20))));

    const struct kefir_list_entry *iter = NULL;
    do {
        iter = kefir_list_head(&specifier->entries);
        ASSERT(iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
        ASSERT(entry->constant != NULL);
        ASSERT(strcmp(entry->constant, "field1") == 0);
        ASSERT(entry->value == NULL);
    } while (0);

    do {
        kefir_list_next(&iter);
        ASSERT(iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
        ASSERT(entry->constant != NULL);
        ASSERT(strcmp(entry->constant, "field2") == 0);
        ASSERT(entry->value != NULL);
        ASSERT(entry->value->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->value->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->value->self)->value.integer == 1);
    } while (0);

    do {
        kefir_list_next(&iter);
        ASSERT(iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
        ASSERT(entry->constant != NULL);
        ASSERT(strcmp(entry->constant, "field3") == 0);
        ASSERT(entry->value != NULL);
        ASSERT(entry->value->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->value->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->value->self)->value.integer == 2);
    } while (0);

    do {
        kefir_list_next(&iter);
        ASSERT(iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
        ASSERT(entry->constant != NULL);
        ASSERT(strcmp(entry->constant, "field4") == 0);
        ASSERT(entry->value == NULL);
    } while (0);

    do {
        kefir_list_next(&iter);
        ASSERT(iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
        ASSERT(entry->constant != NULL);
        ASSERT(strcmp(entry->constant, "field5") == 0);
        ASSERT(entry->value != NULL);
        ASSERT(entry->value->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->value->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->value->self)->value.integer == 20);
    } while (0);

    kefir_list_next(&iter);
    ASSERT(iter == NULL);

    ASSERT_OK(kefir_ast_enum_specifier_free(&kft_mem, specifier));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
