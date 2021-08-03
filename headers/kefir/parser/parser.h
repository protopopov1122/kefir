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

#ifndef KEFIR_PARSER_PARSER_H_
#define KEFIR_PARSER_PARSER_H_

#include "kefir/core/mem.h"
#include "kefir/parser/cursor.h"
#include "kefir/ast/node.h"

typedef struct kefir_parser kefir_parser_t;

typedef struct kefir_parser {
    struct kefir_symbol_table *symbols;
    struct kefir_parser_token_cursor *cursor;
} kefir_parser_t;

typedef kefir_result_t (*kefir_parser_rule_fn_t)(struct kefir_mem *, struct kefir_parser *,
                                                 struct kefir_ast_node_base **, void *);
typedef kefir_result_t (*kefir_parser_invocable_fn_t)(struct kefir_mem *, struct kefir_parser *, void *);

kefir_result_t kefir_parser_init(struct kefir_mem *, struct kefir_parser *, struct kefir_symbol_table *,
                                 struct kefir_parser_token_cursor *);
kefir_result_t kefir_parser_free(struct kefir_mem *, struct kefir_parser *);
kefir_result_t kefir_parser_apply(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **,
                                  kefir_parser_rule_fn_t, void *);
kefir_result_t kefir_parser_try_invoke(struct kefir_mem *, struct kefir_parser *, kefir_parser_invocable_fn_t, void *);

#define KEFIR_PARSER_RULE_FN(_rule) kefir_parser_apply_rule_##_rule
#define KEFIR_PARSER_RULE_APPLY(_mem, _parser, _rule, _result) \
    (kefir_parser_apply((_mem), (_parser), (_result), KEFIR_PARSER_RULE_FN(_rule), NULL))
#define KEFIR_PARSER_NEXT_EXPRESSION(_mem, _parser, _result) \
    KEFIR_PARSER_RULE_APPLY((_mem), (_parser), expression, (_result))

#endif