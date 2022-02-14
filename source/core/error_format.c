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

#include "kefir/core/error_format.h"
#include "kefir/util/json.h"
#include "kefir/core/util.h"
#include "kefir/core/source_error.h"
#include <string.h>

void kefir_format_error_tabular(FILE *out, const struct kefir_error *error) {
    if (out == NULL || error == NULL) {
        return;
    }

    const char *const OVERFLOW_MSG = "[Error stack overflow. Skipping entries]";
    const char *const HEADER_FMT = "%3s  %*s   %9s  %16s   %s\n";
    const char *const ROW_FMT = "%3zu| %*s | %9s| %16s|  %s:%u\n";
    kefir_int_t max_msg_length = 0;
    for (const struct kefir_error *iter = error; iter != NULL; iter = iter->prev_error) {
        kefir_int_t length = strlen(iter->message);
        max_msg_length = MAX(max_msg_length, length);

        if (error->error_overflow) {
            length = strlen(OVERFLOW_MSG);
            max_msg_length = MAX(max_msg_length, length);
        }
    }

    fprintf(out, HEADER_FMT, "No.", -max_msg_length, "Message", "Class", "Subclass", "Compiler ref.");
    for (kefir_size_t i = 0; error != NULL; error = error->prev_error, i++) {
        if (error->error_overflow) {
            fprintf(out, "%3s| %*s | %9s| %16s|\n", "-", -max_msg_length, OVERFLOW_MSG, "", "");
        }

        const char *class = "Unknown";
        const char *subclass = "";
        switch (error->code) {
            case KEFIR_OK:
                class = "Unexpected";
                subclass = "OK";
                break;

            case KEFIR_YIELD:
                class = "Unexpected";
                subclass = "Yield";
                break;

            case KEFIR_ITERATOR_END:
                class = "Internal";
                subclass = "Iterator end";
                break;

            case KEFIR_NO_MATCH:
                class = "Internal";
                subclass = "No match";
                break;

            case KEFIR_NOT_FOUND:
                class = "Internal";
                subclass = "Not found";
                break;

            case KEFIR_ALREADY_EXISTS:
                class = "Internal";
                subclass = "Already exists";
                break;

            case KEFIR_UNKNOWN_ERROR:
                class = "Fatal";
                subclass = "Unknown";
                break;

            case KEFIR_INTERNAL_ERROR:
                class = "Fatal";
                subclass = "Internal";
                break;

            case KEFIR_OS_ERROR:
                class = "Fatal";
                subclass = "Operating System";
                break;

            case KEFIR_INVALID_PARAMETER:
                class = "Fatal";
                subclass = "Invalid parameter";
                break;

            case KEFIR_INVALID_CHANGE:
                class = "Fatal";
                subclass = "Invalid change";
                break;

            case KEFIR_INVALID_REQUEST:
                class = "Fatal";
                subclass = "Invalid request";
                break;

            case KEFIR_INVALID_STATE:
                class = "Fatal";
                subclass = "Invalid state";
                break;

            case KEFIR_OUT_OF_BOUNDS:
                class = "Fatal";
                subclass = "Out of bounds";
                break;

            case KEFIR_OBJALLOC_FAILURE:
                class = "Fatal";
                subclass = "Obj. allocation";
                break;

            case KEFIR_MEMALLOC_FAILURE:
                class = "Fatal";
                subclass = "Mem. allocation";
                break;

            case KEFIR_NOT_SUPPORTED:
                class = "Error";
                subclass = "Not supported";
                break;

            case KEFIR_NOT_IMPLEMENTED:
                class = "Fatal";
                subclass = "Not implemented";
                break;

            case KEFIR_OUT_OF_SPACE:
                class = "Warning";
                subclass = "Out of space";
                break;

            case KEFIR_UI_ERROR:
                class = "Error";
                subclass = "Parameters";
                break;

            case KEFIR_LEXER_ERROR:
                class = "Error";
                subclass = "Lexer";
                break;

            case KEFIR_SYNTAX_ERROR:
                class = "Error";
                subclass = "Parser";
                break;

            case KEFIR_ANALYSIS_ERROR:
                class = "Error";
                subclass = "Analysis";
                break;

            case KEFIR_NOT_CONSTANT:
                class = "Error";
                subclass = "Constant";
                break;

            case KEFIR_STATIC_ASSERT:
                class = "Error";
                subclass = "Static assert";
                break;

            case KEFIR_PREPROCESSOR_ERROR_DIRECTIVE:
                class = "Error";
                subclass = "Preprocessor directive";
                break;
        }

        fprintf(out, ROW_FMT, i, -max_msg_length, error->message, class, subclass, error->file, error->line);
    }
}

static kefir_result_t format_json(FILE *out, const struct kefir_error *error) {
    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, out, 4));
    REQUIRE_OK(kefir_json_output_array_begin(&json));

    for (kefir_size_t i = 0; error != NULL; error = error->prev_error, i++) {
        REQUIRE_OK(kefir_json_output_object_begin(&json));
        REQUIRE_OK(kefir_json_output_object_key(&json, "overflow"));
        REQUIRE_OK(kefir_json_output_boolean(&json, error->error_overflow));
        REQUIRE_OK(kefir_json_output_object_key(&json, "code"));
        switch (error->code) {
            case KEFIR_OK:
                REQUIRE_OK(kefir_json_output_string(&json, "ok"));
                break;

            case KEFIR_YIELD:
                REQUIRE_OK(kefir_json_output_string(&json, "yield"));
                break;

            case KEFIR_ITERATOR_END:
                REQUIRE_OK(kefir_json_output_string(&json, "iterator_end"));
                break;

            case KEFIR_NO_MATCH:
                REQUIRE_OK(kefir_json_output_string(&json, "no_match"));
                break;

            case KEFIR_NOT_FOUND:
                REQUIRE_OK(kefir_json_output_string(&json, "not_found"));
                break;

            case KEFIR_ALREADY_EXISTS:
                REQUIRE_OK(kefir_json_output_string(&json, "already_exists"));
                break;

            case KEFIR_UNKNOWN_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "unknown_error"));
                break;

            case KEFIR_INTERNAL_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "internal_error"));
                break;

            case KEFIR_OS_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "os_error"));
                break;

            case KEFIR_INVALID_PARAMETER:
                REQUIRE_OK(kefir_json_output_string(&json, "invalid_parameter"));
                break;

            case KEFIR_INVALID_CHANGE:
                REQUIRE_OK(kefir_json_output_string(&json, "invalid_change"));
                break;

            case KEFIR_INVALID_REQUEST:
                REQUIRE_OK(kefir_json_output_string(&json, "invalid_request"));
                break;

            case KEFIR_INVALID_STATE:
                REQUIRE_OK(kefir_json_output_string(&json, "invalid_state"));
                break;

            case KEFIR_OUT_OF_BOUNDS:
                REQUIRE_OK(kefir_json_output_string(&json, "out_of_bounds"));
                break;

            case KEFIR_OBJALLOC_FAILURE:
                REQUIRE_OK(kefir_json_output_string(&json, "objalloc_failure"));
                break;

            case KEFIR_MEMALLOC_FAILURE:
                REQUIRE_OK(kefir_json_output_string(&json, "memalloc_failure"));
                break;

            case KEFIR_NOT_SUPPORTED:
                REQUIRE_OK(kefir_json_output_string(&json, "not_supported"));
                break;

            case KEFIR_NOT_IMPLEMENTED:
                REQUIRE_OK(kefir_json_output_string(&json, "not_implemented"));
                break;

            case KEFIR_OUT_OF_SPACE:
                REQUIRE_OK(kefir_json_output_string(&json, "out_of_space"));
                break;

            case KEFIR_UI_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "ui_error"));
                break;

            case KEFIR_LEXER_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "lexer_error"));
                break;

            case KEFIR_SYNTAX_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "syntax_error"));
                break;

            case KEFIR_ANALYSIS_ERROR:
                REQUIRE_OK(kefir_json_output_string(&json, "analysis_error"));
                break;

            case KEFIR_NOT_CONSTANT:
                REQUIRE_OK(kefir_json_output_string(&json, "not_constant"));
                break;

            case KEFIR_STATIC_ASSERT:
                REQUIRE_OK(kefir_json_output_string(&json, "static_assert"));
                break;

            case KEFIR_PREPROCESSOR_ERROR_DIRECTIVE:
                REQUIRE_OK(kefir_json_output_string(&json, "preprocessor_error_directive"));
                break;
        }

        REQUIRE_OK(kefir_json_output_object_key(&json, "message"));
        REQUIRE_OK(kefir_json_output_string(&json, error->message));
        REQUIRE_OK(kefir_json_output_object_key(&json, "origin"));
        REQUIRE_OK(kefir_json_output_object_begin(&json));
        REQUIRE_OK(kefir_json_output_object_key(&json, "file"));
        REQUIRE_OK(kefir_json_output_string(&json, error->file));
        REQUIRE_OK(kefir_json_output_object_key(&json, "line"));
        REQUIRE_OK(kefir_json_output_uinteger(&json, error->line));
        REQUIRE_OK(kefir_json_output_object_end(&json));

        REQUIRE_OK(kefir_json_output_object_key(&json, "source_location"));
        if (error->payload_type == KEFIR_ERROR_PAYLOAD_SOURCE_LOCATION) {
            ASSIGN_DECL_CAST(struct kefir_source_error *, source_error, error->payload);
            if (kefir_source_location_get(&source_error->source_location, NULL, NULL, NULL)) {
                REQUIRE_OK(kefir_json_output_object_begin(&json));
                REQUIRE_OK(kefir_json_output_object_key(&json, "source"));
                REQUIRE_OK(kefir_json_output_string(&json, source_error->source_location.source));
                REQUIRE_OK(kefir_json_output_object_key(&json, "line"));
                REQUIRE_OK(kefir_json_output_uinteger(&json, source_error->source_location.line));
                REQUIRE_OK(kefir_json_output_object_key(&json, "column"));
                REQUIRE_OK(kefir_json_output_uinteger(&json, source_error->source_location.column));
                REQUIRE_OK(kefir_json_output_object_end(&json));
            } else {
                REQUIRE_OK(kefir_json_output_null(&json));
            }
        } else {
            REQUIRE_OK(kefir_json_output_null(&json));
        }

        REQUIRE_OK(kefir_json_output_object_end(&json));
    }

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    return KEFIR_OK;
}

void kefir_format_error_json(FILE *out, const struct kefir_error *error) {
    if (out == NULL || error == NULL) {
        return;
    }
    format_json(out, error);
}
