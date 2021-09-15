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

#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/parser/parser.h"
#include "kefir/parser/rules.h"
#include "kefir/parser/lexer.h"
#include "kefir/parser/format.h"

kefir_result_t make_unit(struct kefir_mem *mem, const struct kefir_ast_context *context,
                         struct kefir_ast_translation_unit **result) {
    const char SOURCE_CODE[] = "float progression(float offset) {\n"
                               "    static float x = 0.0f;\n"
                               "    x += 0.01f;\n"
                               "    return x + offset;\n"
                               "}";

    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_parser_context parser_context;
    struct kefir_lexer lexer;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, SOURCE_CODE, sizeof(SOURCE_CODE), ""));
    REQUIRE_OK(kefir_parser_context_default(&parser_context));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, context->symbols, &source_cursor, &parser_context));
    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_token_buffer_consume(mem, &tokens, &lexer));
    REQUIRE_OK(kefir_lexer_free(mem, &lexer));

    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;

    REQUIRE_OK(kefir_parser_token_cursor_init(&cursor, tokens.tokens, tokens.length));
    REQUIRE_OK(kefir_parser_init(mem, &parser, context->symbols, &cursor));
    struct kefir_ast_node_base *node = NULL;
    REQUIRE_OK(KEFIR_PARSER_NEXT_TRANSLATION_UNIT(mem, &parser, &node));
    *result = node->self;

    REQUIRE_OK(kefir_parser_free(mem, &parser));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_ast_translation_unit *unit = NULL;
    REQUIRE_OK(make_unit(mem, &global_context.context, &unit));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(unit)));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(kefir_ast_translate_unit(mem, KEFIR_AST_NODE_BASE(unit), &translator_context));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_target_platform ir_target;
    REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&ir_target));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(generate_ir(mem, &module, &ir_target));
    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return EXIT_SUCCESS;
}
