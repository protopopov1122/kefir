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

#define _XOPEN_SOURCE 500
#include "kefir/cli/filesystem_source.h"
#include "kefir/cli/input.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/os_error.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>

static kefir_result_t close_source(struct kefir_mem *mem, struct kefir_preprocessor_source_file *source_file) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(source_file != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source file"));

    struct kefir_cli_input *input = source_file->payload;
    REQUIRE_OK(kefir_cli_input_close(mem, input));
    KEFIR_FREE(mem, input);
    *source_file = (struct kefir_preprocessor_source_file){0};
    return KEFIR_OK;
}

static kefir_result_t open_file(struct kefir_mem *mem, const char *filepath, kefir_bool_t system,
                                struct kefir_preprocessor_source_file *source_file,
                                struct kefir_symbol_table *symbols) {
    filepath = kefir_symbol_table_insert(mem, symbols, filepath, NULL);
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert file path into symbol table"));

    REQUIRE(access(filepath, R_OK) == 0, KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Cannot find requested file"));

    struct kefir_cli_input *input = KEFIR_MALLOC(mem, sizeof(struct kefir_cli_input));
    REQUIRE(input != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate file reader"));
    kefir_result_t res = kefir_cli_input_open(mem, input, filepath);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, input);
        return res;
    });

    const char *content = NULL;
    kefir_size_t length;
    res = kefir_cli_input_get(input, &content, &length);
    REQUIRE_CHAIN(&res, kefir_lexer_source_cursor_init(&source_file->cursor, content, length, filepath));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_cli_input_close(mem, input);
        KEFIR_FREE(mem, input);
        return res;
    });

    source_file->filepath = filepath;
    source_file->system = system;
    source_file->close = close_source;
    source_file->payload = input;
    return KEFIR_OK;
}

static kefir_result_t try_open_file(struct kefir_mem *mem, const char *root, const char *filepath, kefir_bool_t system,
                                    struct kefir_preprocessor_source_file *source_file,
                                    struct kefir_preprocessor_filesystem_source_locator *locator) {
    char *path = KEFIR_MALLOC(mem, strlen(root) + strlen(filepath) + 2);
    REQUIRE(path != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate filesystem path"));
    strcpy(path, root);
    strcat(path, "/");
    strcat(path, filepath);
    char *resolved_path = realpath(path, NULL);
    KEFIR_FREE(mem, path);
    kefir_result_t res = open_file(mem, resolved_path, system, source_file, locator->symbols);
    KEFIR_FREE(mem, resolved_path);
    if (res != KEFIR_NOT_FOUND) {
        REQUIRE_OK(res);
        return KEFIR_OK;
    } else {
        return res;
    }
}

static kefir_result_t open_source(struct kefir_mem *mem, const struct kefir_preprocessor_source_locator *source_locator,
                                  const char *filepath, kefir_bool_t system, const char *current_filepath,
                                  struct kefir_preprocessor_source_file *source_file) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(source_locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid filesystem source locator"));
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid file path"));
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to source file"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_filesystem_source_locator *, locator, source_locator);

    if (current_filepath != NULL && !system) {
        char *current_clone = KEFIR_MALLOC(mem, strlen(current_filepath) + 1);
        REQUIRE(current_clone != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate directory name"));
        strcpy(current_clone, current_filepath);
        char *directory = dirname(current_clone);
        REQUIRE_ELSE(directory != NULL, {
            KEFIR_FREE(mem, current_clone);
            return KEFIR_SET_OS_ERROR("Failed to obtain dirname");
        });

        kefir_result_t res = try_open_file(mem, directory, filepath, system, source_file, locator);
        KEFIR_FREE(mem, current_clone);
        if (res != KEFIR_NOT_FOUND) {
            REQUIRE_OK(res);
            return KEFIR_OK;
        }
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&locator->include_roots); iter != NULL;
         kefir_list_next(&iter)) {

        ASSIGN_DECL_CAST(const char *, root, iter->value);
        kefir_result_t res = try_open_file(mem, root, filepath, system, source_file, locator);
        if (res != KEFIR_NOT_FOUND) {
            REQUIRE_OK(res);
            return KEFIR_OK;
        }
    }

    return KEFIR_SET_ERRORF(KEFIR_NOT_FOUND, "Unable to find requested include file %s", filepath);
}

static kefir_result_t free_string(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                  void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(char *, path, entry->value);
    KEFIR_FREE(mem, path);
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_filesystem_source_locator_init(
    struct kefir_preprocessor_filesystem_source_locator *locator, struct kefir_symbol_table *symbols) {
    REQUIRE(locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to filesystem source locator"));
    REQUIRE(symbols != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid symbol table"));

    REQUIRE_OK(kefir_list_init(&locator->include_roots));
    REQUIRE_OK(kefir_list_on_remove(&locator->include_roots, free_string, NULL));
    locator->symbols = symbols;
    locator->locator.payload = &locator;
    locator->locator.open = open_source;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_filesystem_source_locator_free(
    struct kefir_mem *mem, struct kefir_preprocessor_filesystem_source_locator *locator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to filesystem source locator"));

    REQUIRE_OK(kefir_list_free(mem, &locator->include_roots));
    locator->locator.payload = NULL;
    locator->locator.open = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_filesystem_source_locator_append(
    struct kefir_mem *mem, struct kefir_preprocessor_filesystem_source_locator *locator, const char *path) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to filesystem source locator"));
    REQUIRE(path != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid filesystem path"));

    char *copy = KEFIR_MALLOC(mem, strlen(path) + 1);
    REQUIRE(copy != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate filesystem path"));
    strcpy(copy, path);
    kefir_result_t res =
        kefir_list_insert_after(mem, &locator->include_roots, kefir_list_tail(&locator->include_roots), copy);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, copy);
        return res;
    });
    return KEFIR_OK;
}
