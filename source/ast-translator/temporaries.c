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

#include <stdio.h>
#include "kefir/ast-translator/temporaries.h"
#include "kefir/ast-translator/scope/scoped_identifier.h"
#include "kefir/ast/runtime.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_fetch_temporary(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_temporary_identifier *temporary) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(temporary != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST temporary identifier"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(context->ast_context->resolve_ordinary_identifier(
        context->ast_context, KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_id_layout, scoped_id->payload.ptr);

#define BUFFER_LEN 128
    char TEMP_VALUE[BUFFER_LEN] = {0}, TEMP_MEMBER[BUFFER_LEN] = {0};

    snprintf(TEMP_VALUE, BUFFER_LEN - 1, KEFIR_AST_TRANSLATOR_TEMPORARY_VALUE_IDENTIFIER, temporary->identifier);
    snprintf(TEMP_MEMBER, BUFFER_LEN - 1, KEFIR_AST_TRANSLATOR_TEMPORARY_MEMBER_IDENTIFIER, temporary->field);
#undef BUFFER_LEN

    struct kefir_ast_designator temp_value_designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER, .member = TEMP_VALUE, .next = NULL};
    struct kefir_ast_designator temp_member_designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER, .member = TEMP_MEMBER, .next = NULL};

    struct kefir_ast_type_layout *temp_value_layout = NULL, *temp_member_layout = NULL;
    REQUIRE_OK(kefir_ast_type_layout_resolve(scoped_id_layout->layout, &temp_value_designator, &temp_value_layout, NULL,
                                             NULL));
    REQUIRE_OK(
        kefir_ast_type_layout_resolve(temp_value_layout, &temp_member_designator, &temp_member_layout, NULL, NULL));

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_GETLOCALS, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, scoped_id_layout->type_id,
                                               scoped_id_layout->layout->value));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, scoped_id_layout->type_id,
                                               temp_value_layout->value));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, scoped_id_layout->type_id,
                                               temp_member_layout->value));
    return KEFIR_OK;
}
