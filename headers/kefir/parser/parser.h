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

#ifndef KEFIR_PARSER_PARSER_H_
#define KEFIR_PARSER_PARSER_H_

#include "kefir/core/mem.h"
#include "kefir/parser/cursor.h"
#include "kefir/parser/scope.h"
#include "kefir/parser/ruleset.h"
#include "kefir/ast/node.h"

typedef struct kefir_parser kefir_parser_t;

typedef struct kefir_parser_extensions {
    kefir_result_t (*on_init)(struct kefir_mem *, struct kefir_parser *);
    kefir_result_t (*on_free)(struct kefir_mem *, struct kefir_parser *);
    void *payload;
} kefir_parser_extensions_t;

typedef struct kefir_parser {
    struct kefir_symbol_table *symbols;
    struct kefir_parser_token_cursor *cursor;
    struct kefir_parser_scope scope;
    struct kefir_parser_ruleset ruleset;

    const struct kefir_parser_extensions *extensions;
    void *extension_payload;
} kefir_parser_t;

typedef kefir_result_t (*kefir_parser_rule_fn_t)(struct kefir_mem *, struct kefir_parser *,
                                                 struct kefir_ast_node_base **, void *);
typedef kefir_result_t (*kefir_parser_invocable_fn_t)(struct kefir_mem *, struct kefir_parser *, void *);

kefir_result_t kefir_parser_init(struct kefir_mem *, struct kefir_parser *, struct kefir_symbol_table *,
                                 struct kefir_parser_token_cursor *, const struct kefir_parser_extensions *);
kefir_result_t kefir_parser_free(struct kefir_mem *, struct kefir_parser *);
kefir_result_t kefir_parser_apply(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **,
                                  kefir_parser_rule_fn_t, void *);
kefir_result_t kefir_parser_try_invoke(struct kefir_mem *, struct kefir_parser *, kefir_parser_invocable_fn_t, void *);

#define KEFIR_PARSER_RULE_FN(_parser, _rule) ((_parser)->ruleset.rules[KEFIR_PARSER_RULESET_IDENTIFIER(_rule)])
#define KEFIR_PARSER_RULE_APPLY(_mem, _parser, _rule, _result) \
    (kefir_parser_apply((_mem), (_parser), (_result), KEFIR_PARSER_RULE_FN(_parser, _rule), NULL))
#define KEFIR_PARSER_NEXT_EXPRESSION(_mem, _parser, _result) \
    KEFIR_PARSER_RULE_APPLY((_mem), (_parser), expression, (_result))
#define KEFIR_PARSER_NEXT_DECLARATION_LIST(_mem, _parser, _result) \
    KEFIR_PARSER_RULE_APPLY((_mem), (_parser), declaration, (_result))
#define KEFIR_PARSER_NEXT_STATEMENT(_mem, _parser, _result) \
    KEFIR_PARSER_RULE_APPLY((_mem), (_parser), statement, (_result))
#define KEFIR_PARSER_NEXT_TRANSLATION_UNIT(_mem, _parser, _result) \
    KEFIR_PARSER_RULE_APPLY((_mem), (_parser), translation_unit, (_result))

#endif
