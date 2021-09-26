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

#ifndef KEFIR_COMPILER_COMPLIER_H_
#define KEFIR_COMPILER_COMPLIER_H_

#include "kefir/compiler/profile.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/preprocessor/ast_context.h"
#include "kefir/preprocessor/source_file.h"
#include "kefir/lexer/lexer.h"
#include "kefir/ast/node.h"
#include "kefir/ir/module.h"
#include "kefir/core/error.h"

typedef struct kefir_compiler_context {
    struct kefir_compiler_profile *profile;
    const struct kefir_preprocessor_source_locator *source_locator;
    struct kefir_ast_translator_environment translator_env;
    struct kefir_ast_global_context ast_global_context;
    struct kefir_preprocessor_context preprocessor_context;
    struct kefir_preprocessor_ast_context preprocessor_ast_context;
} kefir_compiler_context_t;

struct kefir_mem *kefir_system_memalloc();
kefir_result_t kefir_compiler_context_init(struct kefir_mem *, struct kefir_compiler_context *,
                                           struct kefir_compiler_profile *,
                                           const struct kefir_preprocessor_source_locator *);
kefir_result_t kefir_compiler_context_free(struct kefir_mem *, struct kefir_compiler_context *);
kefir_result_t kefir_compiler_preprocess(struct kefir_mem *, struct kefir_compiler_context *,
                                         struct kefir_token_buffer *, const char *, kefir_size_t, const char *);
kefir_result_t kefir_compiler_preprocess_lex(struct kefir_mem *, struct kefir_compiler_context *,
                                             struct kefir_token_buffer *, const char *, kefir_size_t, const char *);
kefir_result_t kefir_compiler_lex(struct kefir_mem *, struct kefir_compiler_context *, struct kefir_token_buffer *,
                                  const char *, kefir_size_t, const char *);
kefir_result_t kefir_compiler_parse(struct kefir_mem *, struct kefir_compiler_context *, struct kefir_token_buffer *,
                                    struct kefir_ast_translation_unit **);
kefir_result_t kefir_compiler_parse_source(struct kefir_mem *, struct kefir_compiler_context *, const char *,
                                           kefir_size_t, const char *, struct kefir_ast_translation_unit **);
kefir_result_t kefir_compiler_analyze(struct kefir_mem *, struct kefir_compiler_context *,
                                      struct kefir_ast_node_base *);
kefir_result_t kefir_compiler_translate(struct kefir_mem *, struct kefir_compiler_context *,
                                        struct kefir_ast_translation_unit *, struct kefir_ir_module *);
kefir_result_t kefir_compiler_codegen(struct kefir_mem *, struct kefir_compiler_context *, struct kefir_ir_module *,
                                      FILE *);

#endif
