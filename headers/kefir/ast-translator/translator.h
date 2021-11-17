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

#ifndef KEFIR_AST_TRANSLATOR_TRANSLATOR_H_
#define KEFIR_AST_TRANSLATOR_TRANSLATOR_H_

#include "kefir/core/mem.h"
#include "kefir/ast/node.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ir/builder.h"

typedef struct kefir_ast_translator_parameters {
    struct kefir_mem *mem;
    struct kefir_ast_translator_context *context;
    struct kefir_irbuilder_block *builder;
} kefir_ast_translator_parameters_t;

kefir_result_t kefir_ast_translate_object_type(struct kefir_mem *, const struct kefir_ast_type *, kefir_size_t,
                                               const struct kefir_ast_translator_environment *,
                                               struct kefir_irbuilder_type *, struct kefir_ast_type_layout **);

kefir_result_t kefir_ast_translate_expression(struct kefir_mem *, const struct kefir_ast_node_base *,
                                              struct kefir_irbuilder_block *, struct kefir_ast_translator_context *);

kefir_result_t kefir_ast_translate_lvalue(struct kefir_mem *, struct kefir_ast_translator_context *,
                                          struct kefir_irbuilder_block *, const struct kefir_ast_node_base *);

kefir_result_t kefir_ast_translate_statement(struct kefir_mem *, const struct kefir_ast_node_base *,
                                             struct kefir_irbuilder_block *, struct kefir_ast_translator_context *);

kefir_result_t kefir_ast_translate_declaration(struct kefir_mem *, const struct kefir_ast_node_base *,
                                               struct kefir_irbuilder_block *, struct kefir_ast_translator_context *);

kefir_result_t kefir_ast_translate_unit(struct kefir_mem *, const struct kefir_ast_node_base *,
                                        struct kefir_ast_translator_context *);

#endif
