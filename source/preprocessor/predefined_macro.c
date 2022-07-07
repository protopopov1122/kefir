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

static kefir_result_t macro_file_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       const struct kefir_preprocessor_macro *macro, struct kefir_symbol_table *symbols,
                                       const struct kefir_list *args, struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    const char *file = source_location->source;
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_string_literal_multibyte(mem, file, strlen(file) + 1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_line_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       const struct kefir_preprocessor_macro *macro, struct kefir_symbol_table *symbols,
                                       const struct kefir_list *args, struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
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
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_date_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       const struct kefir_preprocessor_macro *macro, struct kefir_symbol_table *symbols,
                                       const struct kefir_list *args, struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, macro->payload);

    struct tm *tm = localtime(&predefined_scope->preprocessor->context->environment.timestamp);
    char strbuf[256] = {0};
    size_t count = strftime(strbuf, sizeof(strbuf), "%b %e %Y", tm);
    REQUIRE(count != 0, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Failed to format current date"));

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_string_literal_multibyte(mem, strbuf, count + 1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_time_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       const struct kefir_preprocessor_macro *macro, struct kefir_symbol_table *symbols,
                                       const struct kefir_list *args, struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, macro->payload);

    struct tm *tm = localtime(&predefined_scope->preprocessor->context->environment.timestamp);
    char strbuf[256] = {0};
    size_t count = strftime(strbuf, sizeof(strbuf), "%H:%M:%S", tm);
    REQUIRE(count != 0, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Failed to format current time"));

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_string_literal_multibyte(mem, strbuf, count + 1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_stdc_hosted_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                              const struct kefir_preprocessor_macro *macro,
                                              struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                              struct kefir_token_buffer *buffer,
                                              const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, macro->payload);

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, predefined_scope->preprocessor->context->environment.hosted ? "1" : "0",
                                         1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_stdc_version_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                               const struct kefir_preprocessor_macro *macro,
                                               struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                               struct kefir_token_buffer *buffer,
                                               const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, macro->payload);

    char strbuf[64] = {0};
    snprintf(strbuf, sizeof(strbuf), KEFIR_ULONG_FMT "L", predefined_scope->preprocessor->context->environment.version);
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, strbuf, strlen(strbuf) + 1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_stdc_iso_10646_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                 const struct kefir_preprocessor_macro *macro,
                                                 struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                                 struct kefir_token_buffer *buffer,
                                                 const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, macro->payload);

    char strbuf[64] = {0};
    snprintf(strbuf, sizeof(strbuf), KEFIR_ULONG_FMT "L",
             predefined_scope->preprocessor->context->environment.stdc_iso10646);
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, strbuf, strlen(strbuf) + 1, &token));
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_stdc_lib_ext1_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                const struct kefir_preprocessor_macro *macro,
                                                struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                                struct kefir_token_buffer *buffer,
                                                const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, macro->payload);

    char strbuf[64] = {0};
    snprintf(strbuf, sizeof(strbuf), KEFIR_ULONG_FMT "L",
             predefined_scope->preprocessor->context->environment.stdc_lib_ext1);
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, strbuf, strlen(strbuf) + 1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_produce_one_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                              const struct kefir_preprocessor_macro *macro,
                                              struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                              struct kefir_token_buffer *buffer,
                                              const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, "1", 1, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_big_endian_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                             const struct kefir_preprocessor_macro *macro,
                                             struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                             struct kefir_token_buffer *buffer,
                                             const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, "1234", 4, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_little_endian_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                const struct kefir_preprocessor_macro *macro,
                                                struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                                struct kefir_token_buffer *buffer,
                                                const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, "4321", 4, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_pdp_endian_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                             const struct kefir_preprocessor_macro *macro,
                                             struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                             struct kefir_token_buffer *buffer,
                                             const struct kefir_source_location *source_location) {
    UNUSED(symbols);
    UNUSED(preprocessor);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(source_location != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source location"));

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, "3412", 4, &token));
    token.source_location = *source_location;
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t define_predefined_macro(
    struct kefir_mem *mem, struct kefir_preprocessor_predefined_macro_scope *scope,
    struct kefir_preprocessor_macro *macro, const char *identifier,
    kefir_result_t (*apply)(struct kefir_mem *, struct kefir_preprocessor *, const struct kefir_preprocessor_macro *,
                            struct kefir_symbol_table *, const struct kefir_list *, struct kefir_token_buffer *,
                            const struct kefir_source_location *)) {
    macro->identifier = identifier;
    macro->type = KEFIR_PREPROCESSOR_MACRO_OBJECT;
    macro->payload = scope;
    macro->apply = apply;
    macro->argc = predefined_macro_argc;

    REQUIRE_OK(kefir_hashtree_insert(mem, &scope->macro_tree, (kefir_hashtree_key_t) identifier,
                                     (kefir_hashtree_value_t) macro));
    return KEFIR_OK;
}

static kefir_result_t locate_predefined(const struct kefir_preprocessor_macro_scope *scope, const char *identifier,
                                        const struct kefir_preprocessor_macro **macro) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid overlay macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, scope->payload);

    struct kefir_hashtree_node *node;
    kefir_result_t res = kefir_hashtree_at(&predefined_scope->macro_tree, (kefir_hashtree_key_t) identifier, &node);
    if (res == KEFIR_NOT_FOUND) {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Requested identifier was not found in predefined macro scope");
    } else {
        REQUIRE_OK(res);
        *macro = (void *) node->value;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_predefined_macro_scope_init(struct kefir_mem *mem,
                                                              struct kefir_preprocessor_predefined_macro_scope *scope,
                                                              struct kefir_preprocessor *preprocessor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to predefined macro scope"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    scope->preprocessor = preprocessor;
    scope->scope.payload = scope;
    scope->scope.locate = locate_predefined;
    REQUIRE_OK(kefir_hashtree_init(&scope->macro_tree, &kefir_hashtree_str_ops));

    kefir_result_t res = define_predefined_macro(mem, scope, &scope->macros.file, "__FILE__", macro_file_apply);
    REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.line, "__LINE__", macro_line_apply));
    REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.date, "__DATE__", macro_date_apply));
    REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.time, "__TIME__", macro_time_apply));
    REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc, "__STDC__", macro_produce_one_apply));
    REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_hosted, "__STDC_HOSTED__",
                                                macro_stdc_hosted_apply));
    REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_version, "__STDC_VERSION__",
                                                macro_stdc_version_apply));

    if (preprocessor->context->environment.stdc_iso10646 > 0) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_iso_10646, "__STDC_ISO_10646__",
                                                    macro_stdc_iso_10646_apply));
    }
    if (preprocessor->context->environment.stdc_mb_might_neq_wc) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_mb_might_neq_wc,
                                                    "__STDC_MB_MIGHT_NEQ_WC__", macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_utf16) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_utf16, "__STDC_UTF_16__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_utf32) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_utf32, "__STDC_UTF_32__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_analyzable) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_analyzable, "__STDC_ANALYZABLE__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_iec559) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_iec559, "__STDC_IEC_559__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_iec559_complex) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_iec559_complex,
                                                    "__STDC_IEC_559_COMPLEX__", macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_lib_ext1 > 0) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_lib_ext1, "__STDC_LIB_EXT1__",
                                                    macro_stdc_lib_ext1_apply));
    }
    if (preprocessor->context->environment.stdc_no_atomics) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_no_atomics, "__STDC_NO_ATOMICS__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_no_complex) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_no_complex, "__STDC_NO_COMPLEX__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_no_threads) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_no_threads, "__STDC_NO_THREADS__",
                                                    macro_produce_one_apply));
    }
    if (preprocessor->context->environment.stdc_no_vla) {
        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.stdc_no_vla, "__STDC_NO_VLA__",
                                                    macro_produce_one_apply));
    }
    REQUIRE_CHAIN(&res,
                  define_predefined_macro(mem, scope, &scope->macros.kefircc, "__KEFIRCC__", macro_produce_one_apply));

    if (preprocessor->context->environment.data_model != NULL) {
        switch (preprocessor->context->environment.data_model->model) {
            case KEFIR_DATA_MODEL_UNKNOWN:
                // Intentionally left blank
                break;

            case KEFIR_DATA_MODEL_ILP32:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.data_model, "__ILP32__",
                                                            macro_produce_one_apply));
                break;

            case KEFIR_DATA_MODEL_LLP64:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.data_model, "__LLP64__",
                                                            macro_produce_one_apply));
                break;

            case KEFIR_DATA_MODEL_LP64:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.data_model, "__LP64__",
                                                            macro_produce_one_apply));
                break;

            case KEFIR_DATA_MODEL_ILP64:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.data_model, "__ILP64__",
                                                            macro_produce_one_apply));
                break;

            case KEFIR_DATA_MODEL_SILP64:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.data_model, "__SILP64__",
                                                            macro_produce_one_apply));
                break;
        }

        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.byte_order_big_endian,
                                                    "__ORDER_BIG_ENDIAN__", macro_big_endian_apply));

        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.byte_order_little_endian,
                                                    "__ORDER_LITTLE_ENDIAN__", macro_little_endian_apply));

        REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.byte_order_pdp_endian,
                                                    "__ORDER_PDP_ENDIAN__", macro_pdp_endian_apply));
        switch (preprocessor->context->environment.data_model->byte_order) {
            case KEFIR_BYTE_ORDER_BIG_ENDIAN:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.byte_order, "__BYTE_ORDER__",
                                                            macro_big_endian_apply));
                break;

            case KEFIR_BYTE_ORDER_LITTLE_ENDIAN:
                REQUIRE_CHAIN(&res, define_predefined_macro(mem, scope, &scope->macros.byte_order, "__BYTE_ORDER__",
                                                            macro_little_endian_apply));
                break;

            case KEFIR_BYTE_ORDER_UNKNOWN:
                // Intentionally left blank
                break;
        }
    }

    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &scope->macro_tree);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_predefined_macro_scope_free(struct kefir_mem *mem,
                                                              struct kefir_preprocessor_predefined_macro_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to predefined macro scope"));
    REQUIRE_OK(kefir_hashtree_free(mem, &scope->macro_tree));
    return KEFIR_OK;
}
