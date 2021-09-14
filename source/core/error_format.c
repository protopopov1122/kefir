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
#include "kefir/core/util.h"
#include <string.h>

void kefir_format_error(FILE *out, const struct kefir_error *error) {
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
                class = "Fatal";
                subclass = "Not supported";
                break;

            case KEFIR_NOT_IMPLEMENTED:
                class = "Fatal";
                subclass = "Not implemented";
                break;

            case KEFIR_OUT_OF_SPACE:
                class = "Fatal";
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
        }

        fprintf(out, ROW_FMT, i, -max_msg_length, error->message, class, subclass, error->file, error->line);
    }
}
