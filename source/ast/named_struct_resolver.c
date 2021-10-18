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

#include "kefir/ast/context.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

static kefir_result_t resolve_named_structure(const char *identifier, const struct kefir_ast_type **type_ptr,
                                              void *payload) {
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure identifier"));
    REQUIRE(type_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST type"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(const struct kefir_ast_context *, context, payload);

    const struct kefir_ast_scoped_identifier *struct_scoped_id = NULL;
    REQUIRE_OK(context->resolve_tag_identifier(context, identifier, &struct_scoped_id));
    REQUIRE(struct_scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG &&
                (struct_scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
                 struct_scoped_id->type->tag == KEFIR_AST_TYPE_UNION) &&
                struct_scoped_id->type->structure_type.complete,
            KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to find complete named structure type"));
    *type_ptr = struct_scoped_id->type;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_context_named_structure_resolver_init(const struct kefir_ast_context *context,
                                                               struct kefir_ast_named_structure_resolver *resolver) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(resolver != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to named structure resolver"));

    resolver->resolve = resolve_named_structure;
    resolver->payload = (void *) context;
    return KEFIR_OK;
}
