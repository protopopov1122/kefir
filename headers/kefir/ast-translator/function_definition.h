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

#ifndef KEFIR_AST_TRANSLATOR_FUNCTION_DEFINITION_H_
#define KEFIR_AST_TRANSLATOR_FUNCTION_DEFINITION_H_

#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/type_resolver.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"

struct kefir_ast_translator_function_context {
    struct kefir_ast_function_definition *function_definition;
    struct kefir_ir_module *module;
    const struct kefir_ast_translator_resolved_type *resolved_function_type;
    struct kefir_ast_translator_function_declaration *function_declaration;
    struct kefir_ast_local_context *local_context;
    struct kefir_ast_translator_context local_translator_context;
    struct kefir_ast_translator_local_scope_layout local_scope_layout;
    struct kefir_ir_function *ir_func;
    struct kefir_irbuilder_block builder;
};

kefir_result_t kefir_ast_translator_function_context_init(struct kefir_mem *, struct kefir_ast_translator_context *,
                                                          struct kefir_ast_function_definition *,
                                                          struct kefir_ast_translator_function_context *);
kefir_result_t kefir_ast_translator_function_context_free(struct kefir_mem *,
                                                          struct kefir_ast_translator_function_context *);
kefir_result_t kefir_ast_translator_function_context_translate(struct kefir_mem *,
                                                               struct kefir_ast_translator_function_context *);
kefir_result_t kefir_ast_translator_function_context_finalize(struct kefir_mem *,
                                                              struct kefir_ast_translator_function_context *);

#endif
