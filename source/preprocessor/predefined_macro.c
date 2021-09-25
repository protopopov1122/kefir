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

#include "kefir/preprocessor/predefined_macro.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t predefined_macro_argc(const struct kefir_preprocessor_macro *macro, kefir_size_t *argc_ptr,
                                            kefir_bool_t *vararg_ptr) {
    UNUSED(macro);
    UNUSED(argc_ptr);
    UNUSED(vararg_ptr);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Unable to retrieve argument count of predefined object macro");
}

static kefir_result_t macro_file_apply(struct kefir_mem *mem, const struct kefir_preprocessor_macro *macro,
                                       struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                       struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    const char *file = source_location->source;
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_string_literal_multibyte(mem, file, strlen(file) + 1, &token));
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_line_apply(struct kefir_mem *mem, const struct kefir_preprocessor_macro *macro,
                                       struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                       struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    kefir_source_location_line_t line = source_location->line;
    char strbuf[64] = {0};
    snprintf(strbuf, sizeof(strbuf), KEFIR_UINT_FMT, line);
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, strbuf, strlen(strbuf) + 1, &token));
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t define_predefined_macro(
    struct kefir_preprocessor_predefined_macro_scope *scope, struct kefir_preprocessor_macro *macro,
    const char *identifier,
    kefir_result_t (*apply)(struct kefir_mem *, const struct kefir_preprocessor_macro *, struct kefir_symbol_table *,
                            const struct kefir_list *, struct kefir_token_buffer *,
                            const struct kefir_source_location *)) {
    macro->identifier = identifier;
    macro->type = KEFIR_PREPROCESSOR_MACRO_OBJECT;
    macro->payload = scope;
    macro->apply = apply;
    macro->argc = predefined_macro_argc;
    return KEFIR_OK;
}

static kefir_result_t locate_predefined(const struct kefir_preprocessor_macro_scope *scope, const char *identifier,
                                        const struct kefir_preprocessor_macro **macro) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid overlay macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, scope->payload);
    UNUSED(predefined_scope);

    if (strcmp(identifier, "__FILE__") == 0) {
        *macro = &predefined_scope->macros.file;
    } else if (strcmp(identifier, "__LINE__") == 0) {
        *macro = &predefined_scope->macros.line;
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Requested identifier was not found in predefined macro scope");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_predefined_macro_scope_init(struct kefir_preprocessor_predefined_macro_scope *scope,
                                                              struct kefir_preprocessor *preprocessor) {
    REQUIRE(scope != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to predefined macro scope"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    scope->preprocessor = preprocessor;
    scope->scope.payload = scope;
    scope->scope.locate = locate_predefined;

    REQUIRE_OK(define_predefined_macro(scope, &scope->macros.file, "__FILE__", macro_file_apply));
    REQUIRE_OK(define_predefined_macro(scope, &scope->macros.line, "__LINE__", macro_line_apply));
    return KEFIR_OK;
}
