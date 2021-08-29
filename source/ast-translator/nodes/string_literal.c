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

#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_string_literal_node(struct kefir_mem *mem,
                                                       struct kefir_ast_translator_context *context,
                                                       struct kefir_irbuilder_block *builder,
                                                       const struct kefir_ast_string_literal *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST string literal node"));

    kefir_id_t literal_id;
    static const kefir_ir_string_literal_type_t LiteralTypes[] = {
        [KEFIR_AST_STRING_LITERAL_MULTIBYTE] = KEFIR_IR_STRING_LITERAL_MULTIBYTE,
        [KEFIR_AST_STRING_LITERAL_UNICODE8] = KEFIR_IR_STRING_LITERAL_MULTIBYTE,
        [KEFIR_AST_STRING_LITERAL_UNICODE16] = KEFIR_IR_STRING_LITERAL_UNICODE16,
        [KEFIR_AST_STRING_LITERAL_UNICODE32] = KEFIR_IR_STRING_LITERAL_UNICODE32,
        [KEFIR_AST_STRING_LITERAL_WIDE] = KEFIR_IR_STRING_LITERAL_UNICODE32};
    REQUIRE_OK(kefir_ir_module_string_literal(mem, context->module, LiteralTypes[node->type], node->literal,
                                              node->length, &literal_id));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHSTRING, literal_id));
    return KEFIR_OK;
}
