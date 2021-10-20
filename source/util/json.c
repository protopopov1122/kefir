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

#include "kefir/util/json.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>
#include <uchar.h>

kefir_result_t kefir_json_output_init(struct kefir_json_output *json, FILE *file, kefir_size_t indent) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid json output"));
    REQUIRE(file != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid file"));

    json->file = file;
    json->level = 0;
    json->indent = indent;
    memset(json->state, KEFIR_JSON_STATE_INIT, KEFIR_JSON_MAX_DEPTH * sizeof(kefir_json_state_t));
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_finalize(struct kefir_json_output *json) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid json output"));
    REQUIRE(json->level == 0, KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot finalizer malformed json"));

    fflush(json->file);
    json->file = NULL;
    return KEFIR_OK;
}

static inline kefir_result_t valid_json(struct kefir_json_output *json) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid json output"));
    REQUIRE(json->file != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid json output"));
    return KEFIR_OK;
}

static inline kefir_result_t write_indent(struct kefir_json_output *json) {
    if (json->indent > 0) {
        fprintf(json->file, "\n");
        for (kefir_size_t i = 0; i < json->indent * json->level; i++) {
            fprintf(json->file, " ");
        }
    }
    return KEFIR_OK;
}

static inline kefir_result_t write_separator(struct kefir_json_output *json) {
    switch (json->state[json->level]) {
        case KEFIR_JSON_STATE_OBJECT_EMPTY:
        case KEFIR_JSON_STATE_ARRAY_EMPTY:
            write_indent(json);
            break;

        case KEFIR_JSON_STATE_INIT:
            // Intentionally left blank
            break;

        case KEFIR_JSON_STATE_OBJECT_FIELD:
            fprintf(json->file, ":");
            if (json->indent > 0) {
                fprintf(json->file, " ");
            }
            break;

        case KEFIR_JSON_STATE_OBJECT:
        case KEFIR_JSON_STATE_ARRAY:
            fprintf(json->file, ",");
            write_indent(json);
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_object_begin(struct kefir_json_output *json) {
    REQUIRE_OK(valid_json(json));
    REQUIRE(json->level + 1 < KEFIR_JSON_MAX_DEPTH,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Maximum json depth exceeded"));

    REQUIRE_OK(write_separator(json));
    switch (json->state[json->level]) {
        case KEFIR_JSON_STATE_INIT:
        case KEFIR_JSON_STATE_ARRAY:
            // Intentionally left blank
            break;

        case KEFIR_JSON_STATE_OBJECT_FIELD:
            json->state[json->level] = KEFIR_JSON_STATE_OBJECT;
            break;

        case KEFIR_JSON_STATE_ARRAY_EMPTY:
            json->state[json->level] = KEFIR_JSON_STATE_ARRAY;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot open json object");
    }
    json->state[++json->level] = KEFIR_JSON_STATE_OBJECT_EMPTY;
    fprintf(json->file, "{");
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_object_end(struct kefir_json_output *json) {
    REQUIRE_OK(valid_json(json));
    REQUIRE(json->level > 0, KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot close json object"));
    REQUIRE(json->state[json->level] == KEFIR_JSON_STATE_OBJECT_EMPTY ||
                json->state[json->level] == KEFIR_JSON_STATE_OBJECT,
            KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot close json object"));

    kefir_bool_t indent = json->state[json->level] == KEFIR_JSON_STATE_OBJECT;
    json->level--;
    if (indent) {
        write_indent(json);
    }
    fprintf(json->file, "}");
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_array_begin(struct kefir_json_output *json) {
    REQUIRE_OK(valid_json(json));
    REQUIRE(json->level + 1 < KEFIR_JSON_MAX_DEPTH,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Maximum json depth exceeded"));

    REQUIRE_OK(write_separator(json));
    switch (json->state[json->level]) {
        case KEFIR_JSON_STATE_INIT:
        case KEFIR_JSON_STATE_ARRAY:
            // Intentionally left blank
            break;

        case KEFIR_JSON_STATE_OBJECT_FIELD:
            json->state[json->level] = KEFIR_JSON_STATE_OBJECT;
            break;

        case KEFIR_JSON_STATE_ARRAY_EMPTY:
            json->state[json->level] = KEFIR_JSON_STATE_ARRAY;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot open json array");
    }
    json->state[++json->level] = KEFIR_JSON_STATE_ARRAY_EMPTY;
    fprintf(json->file, "[");
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_array_end(struct kefir_json_output *json) {
    REQUIRE_OK(valid_json(json));
    REQUIRE(json->level > 0, KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot close json array"));
    REQUIRE(
        json->state[json->level] == KEFIR_JSON_STATE_ARRAY_EMPTY || json->state[json->level] == KEFIR_JSON_STATE_ARRAY,
        KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot close json array"));

    kefir_bool_t indent = json->state[json->level] == KEFIR_JSON_STATE_ARRAY;
    json->level--;
    if (indent) {
        write_indent(json);
    }
    fprintf(json->file, "]");
    return KEFIR_OK;
}

static kefir_result_t format_string(struct kefir_json_output *json, const char *string) {
    fprintf(json->file, "\"");

    mbstate_t state = {0};
    const char *end = string + strlen(string);
    size_t sz = 0;
    kefir_char32_t wide_char;
    while (string < end && (sz = mbrtoc32(&wide_char, string, end - string, &state)) != 0) {
        switch (wide_char) {
            case U'\"':
                fprintf(json->file, "\\\"");
                break;

            case U'\\':
                fprintf(json->file, "\\\\");
                break;

            case U'/':
                fprintf(json->file, "\\/");
                break;

            case U'\b':
                fprintf(json->file, "\\b");
                break;

            case U'\f':
                fprintf(json->file, "\\f");
                break;

            case U'\n':
                fprintf(json->file, "\\n");
                break;

            case U'\r':
                fprintf(json->file, "\\r");
                break;

            case U'\t':
                fprintf(json->file, "\\t");
                break;

            case U'\a':
                fprintf(json->file, "\\a");
                break;

            default:
                fwrite(string, 1, sz, json->file);
                break;
        }
        string += sz;
    }
    fprintf(json->file, "\"");
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_object_key(struct kefir_json_output *json, const char *key) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));

    REQUIRE(json->state[json->level] == KEFIR_JSON_STATE_OBJECT_EMPTY ||
                json->state[json->level] == KEFIR_JSON_STATE_OBJECT,
            KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot add key to json object"));

    json->state[json->level] = KEFIR_JSON_STATE_OBJECT_FIELD;
    REQUIRE_OK(format_string(json, key));
    return KEFIR_OK;
}

static kefir_result_t validate_value(struct kefir_json_output *json) {
    switch (json->state[json->level]) {
        case KEFIR_JSON_STATE_OBJECT_FIELD:
            json->state[json->level] = KEFIR_JSON_STATE_OBJECT;
            break;

        case KEFIR_JSON_STATE_ARRAY_EMPTY:
            json->state[json->level] = KEFIR_JSON_STATE_ARRAY;
            break;

        case KEFIR_JSON_STATE_INIT:
        case KEFIR_JSON_STATE_ARRAY:
            // Intentionally left blank
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Cannot write json value");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_integer(struct kefir_json_output *json, kefir_int64_t value) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, KEFIR_INT64_FMT, value);
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_uinteger(struct kefir_json_output *json, kefir_uint64_t value) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, KEFIR_UINT64_FMT, value);
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_float(struct kefir_json_output *json, kefir_float64_t value) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, KEFIR_FLOAT64_FMT, value);
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_hexfloat(struct kefir_json_output *json, kefir_float64_t value) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, "\"%a\"", value);
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_string(struct kefir_json_output *json, const char *value) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    REQUIRE_OK(format_string(json, value));
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_raw_string(struct kefir_json_output *json, const char *value, kefir_size_t length) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, "\"");
    for (kefir_size_t i = 0; i < length; i++) {
        char chr = value[i];
        switch (chr) {
            case U'\"':
                fprintf(json->file, "\\\"");
                break;

            case U'\\':
                fprintf(json->file, "\\\\");
                break;

            case U'/':
                fprintf(json->file, "\\/");
                break;

            case U'\b':
                fprintf(json->file, "\\b");
                break;

            case U'\f':
                fprintf(json->file, "\\f");
                break;

            case U'\n':
                fprintf(json->file, "\\n");
                break;

            case U'\r':
                fprintf(json->file, "\\r");
                break;

            case U'\t':
                fprintf(json->file, "\\t");
                break;

            case U'\a':
                fprintf(json->file, "\\a");
                break;

            case U'\v':
                fprintf(json->file, "\\v");
                break;

            case U'\?':
                fprintf(json->file, "\\?");
                break;

            case U'\0':
                fprintf(json->file, "\\u0000");
                break;

            default:
                fputc(chr, json->file);
                break;
        }
    }
    fprintf(json->file, "\"");
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_boolean(struct kefir_json_output *json, kefir_bool_t value) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, "%s", value ? "true" : "false");
    return KEFIR_OK;
}

kefir_result_t kefir_json_output_null(struct kefir_json_output *json) {
    REQUIRE_OK(valid_json(json));
    REQUIRE_OK(write_separator(json));
    REQUIRE_OK(validate_value(json));

    fprintf(json->file, "null");
    return KEFIR_OK;
}
