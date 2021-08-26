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

kefir_result_t kefir_ast_translate_constant_node(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                 struct kefir_irbuilder_block *builder,
                                                 const struct kefir_ast_constant *node) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant node"));

    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                   (kefir_uint64_t) node->value.boolean);

        case KEFIR_AST_CHAR_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64,
                                                   (kefir_int64_t) node->value.character);

        case KEFIR_AST_WIDE_CHAR_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64,
                                                   (kefir_int64_t) node->value.wide_character);

        case KEFIR_AST_UNICODE16_CHAR_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                   (kefir_uint64_t) node->value.unicode16_character);

        case KEFIR_AST_UNICODE32_CHAR_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                   (kefir_uint64_t) node->value.unicode32_character);

        case KEFIR_AST_INT_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64,
                                                   (kefir_int64_t) node->value.integer);

        case KEFIR_AST_UINT_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                   (kefir_uint64_t) node->value.uinteger);

        case KEFIR_AST_LONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64,
                                                   (kefir_int64_t) node->value.long_integer);

        case KEFIR_AST_ULONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                   (kefir_uint64_t) node->value.ulong_integer);

        case KEFIR_AST_LONG_LONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PUSHI64,
                                                   (kefir_int64_t) node->value.long_long);

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PUSHU64,
                                                   (kefir_uint64_t) node->value.ulong_long);

        case KEFIR_AST_FLOAT_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDF32(builder, KEFIR_IROPCODE_PUSHF32, node->value.float32, 0.0f);

        case KEFIR_AST_DOUBLE_CONSTANT:
            return KEFIR_IRBUILDER_BLOCK_APPENDF64(builder, KEFIR_IROPCODE_PUSHF64, node->value.float64);

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected AST constant type");
    }
}
