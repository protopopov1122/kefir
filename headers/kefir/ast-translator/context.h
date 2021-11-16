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

#ifndef KEFIR_AST_TRANSLATOR_CONTEXT_H_
#define KEFIR_AST_TRANSLATOR_CONTEXT_H_

#include "kefir/ast/context.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ast-translator/type_cache.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ir/module.h"

typedef struct kefir_ast_translator_context kefir_ast_translator_context_t;

typedef struct kefir_ast_translator_context_extensions {
    kefir_result_t (*on_init)(struct kefir_mem *, struct kefir_ast_translator_context *);
    kefir_result_t (*on_free)(struct kefir_mem *, struct kefir_ast_translator_context *);
    void *payload;
} kefir_ast_translator_context_extensions_t;

typedef struct kefir_ast_translator_context {
    struct kefir_ast_translator_context *base_context;
    const struct kefir_ast_context *ast_context;
    const struct kefir_ast_translator_environment *environment;
    struct kefir_ir_module *module;
    struct kefir_ast_translator_type_cache type_cache;

    struct kefir_ast_translator_global_scope_layout *global_scope_layout;
    struct kefir_ast_translator_local_scope_layout *local_scope_layout;

    const struct kefir_ast_translator_context_extensions *extensions;
    void *extensions_payload;
} kefir_ast_translator_context_t;

kefir_result_t kefir_ast_translator_context_init(struct kefir_mem *, struct kefir_ast_translator_context *,
                                                 const struct kefir_ast_context *,
                                                 const struct kefir_ast_translator_environment *,
                                                 struct kefir_ir_module *,
                                                 const struct kefir_ast_translator_context_extensions *);

kefir_result_t kefir_ast_translator_context_init_local(struct kefir_mem *, struct kefir_ast_translator_context *,
                                                       const struct kefir_ast_context *,
                                                       struct kefir_ast_translator_context *);

kefir_result_t kefir_ast_translator_context_free(struct kefir_mem *, struct kefir_ast_translator_context *);

struct kefir_ast_translator_type_resolver *kefir_ast_translator_context_type_resolver(
    struct kefir_ast_translator_context *);

#endif
