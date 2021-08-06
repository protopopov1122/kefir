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

#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t KEFIR_PARSER_RULE_FN(type_name)(struct kefir_mem *mem, struct kefir_parser *parser,
                                               struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    struct kefir_ast_declarator_specifier_list specifiers;
    struct kefir_ast_declarator *declarator = NULL;

    REQUIRE_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    kefir_result_t res = kefir_parser_scan_declaration_specifier_list(mem, parser, &specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return res;
    });

    res = kefir_parser_scan_abstract_declarator(mem, parser, &declarator);
    if (res == KEFIR_NO_MATCH) {
        declarator = kefir_ast_declarator_identifier(mem, NULL, NULL);
        REQUIRE_ELSE(declarator != NULL, {
            kefir_ast_declarator_specifier_list_free(mem, &specifiers);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate empty AST identifier declarator");
        });
    } else {
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_declarator_specifier_list_free(mem, &specifiers);
            return res;
        });
    }

    struct kefir_ast_type_name *type_name = kefir_ast_new_type_name(mem, declarator);
    REQUIRE_ELSE(type_name != NULL, {
        kefir_ast_declarator_free(mem, declarator);
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST type name");
    });

    res = kefir_ast_declarator_specifier_list_clone(mem, &type_name->type_decl.specifiers, &specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(type_name));
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return res;
    });

    res = kefir_ast_declarator_specifier_list_free(mem, &specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(type_name));
        return res;
    });

    *result = KEFIR_AST_NODE_BASE(type_name);
    return KEFIR_OK;
}
