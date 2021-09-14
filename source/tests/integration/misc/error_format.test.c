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
#include "kefir/core/os_error.h"
#include "kefir/core/source_error.h"
#include "kefir/core/util.h"
#include "kefir/core/mem.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    UNUSED(mem);
    kefir_format_error(stdout, kefir_current_error());
    printf("\n\n");

    KEFIR_SET_ERROR(KEFIR_OK, "Should now show up #1");
    KEFIR_SET_ERROR(KEFIR_YIELD, "Should now show up #2");
    KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Should now show up #3");
    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Should now show up #4");
    KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Should now show up #5");
    KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Should now show up #6");
    KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Some unknown error here");
    KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Total internal failure");
    KEFIR_SET_ERROR(KEFIR_OS_ERROR, "Error from OS");
    KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "I've got invalid parameter");
    KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Invalid change was requested");
    KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Invalid request was made");
    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Invalid state was detected");
    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Out of bounds issue");
    KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Object allocation failed");
    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Memory allocation failed");
    KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Feature is not supported");
    KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Feature is not implemented yet");
    KEFIR_SET_ERROR(KEFIR_OUT_OF_SPACE, "Out of space issue");
    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Unexpected option");
    KEFIR_SET_ERROR(KEFIR_LEXER_ERROR, "Lexing issue");
    KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Syntax error detected");
    KEFIR_SET_ERROR(KEFIR_ANALYSIS_ERROR, "Analysis error detected");
    KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Constant expected");
    KEFIR_SET_ERROR(KEFIR_STATIC_ASSERT, "Static assertion failed");
    kefir_format_error(stdout, kefir_current_error());
    kefir_clear_error();
    printf("\n\n");

    KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Iterator end");
    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "No match found");
    KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Entry not found");
    KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Entry already exists");
    kefir_format_error(stdout, kefir_current_error());
    kefir_clear_error();
    printf("\n\n");

    KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Entry already exists");
    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "No match found");
    KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Iterator end");
    KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Entry not found");
    kefir_format_error(stdout, kefir_current_error());
    kefir_clear_error();
    printf("\n\n");

    KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Entry not found");
    KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Iterator end");
    KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Entry already exists");
    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "No match found");
    kefir_format_error(stdout, kefir_current_error());
    kefir_clear_error();
    printf("\n\n");

    KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Entry already exists");
    KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Entry not found");
    KEFIR_SET_ERROR(KEFIR_NO_MATCH, "No match found");
    KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Iterator end");
    kefir_format_error(stdout, kefir_current_error());
    kefir_clear_error();
    printf("\n\n");

    struct kefir_source_location location1 = {.source = "file1.c", .line = 100, .column = 5};
    KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &location1, "Lexer error detected");
    struct kefir_source_location location2 = {.source = "file2.c", .line = 1, .column = 10};
    KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, &location2, "Parsing error detected");
    struct kefir_source_location location3 = {.source = "file3.c", .line = 4739, .column = 1};
    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &location3, "Analyzer error detected");
    struct kefir_source_location location4 = {.source = "file4.c", .line = 91, .column = 1000};
    KEFIR_SET_SOURCE_ERROR(KEFIR_NOT_CONSTANT, &location4, "Expected constant");
    struct kefir_source_location location5 = {.source = "file5.c", .line = 1, .column = 1};
    KEFIR_SET_SOURCE_ERROR(KEFIR_STATIC_ASSERT, &location5, "Static assertion failed");
    kefir_format_error(stdout, kefir_current_error());
    kefir_clear_error();
    printf("\n\n");

    char BUFFER[256][KEFIR_ERROR_STACK_SIZE + 10];
    for (kefir_size_t i = 0; i < KEFIR_ERROR_STACK_SIZE + 10; i++) {
        snprintf(BUFFER[i], 256, "Some unknwon error #%zu", i);
        KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, BUFFER[i]);
    }
    kefir_format_error(stdout, kefir_current_error());
    return KEFIR_OK;
}
