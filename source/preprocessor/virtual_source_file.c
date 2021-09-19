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

#include "kefir/preprocessor/virtual_source_file.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

static kefir_result_t close_source(struct kefir_mem *mem, struct kefir_preprocessor_source_file *source_file) {
    UNUSED(mem);
    REQUIRE(source_file != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source file"));
    *source_file = (struct kefir_preprocessor_source_file){0};
    return KEFIR_OK;
}

static kefir_result_t open_source(struct kefir_mem *mem, const struct kefir_preprocessor_source_locator *source_locator,
                                  const char *filepath, kefir_bool_t system,
                                  struct kefir_preprocessor_source_file *source_file) {
    UNUSED(mem);
    REQUIRE(source_locator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid virtual source locator"));
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid file path"));
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to source file"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_virtual_source_locator *, locator, source_locator);

    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&locator->sources, (kefir_hashtree_key_t) filepath, &node));
    source_file->filepath = filepath;
    source_file->system = system;
    source_file->payload = (void *) node->value;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_file->cursor, source_file->payload, strlen(source_file->payload),
                                              filepath));
    source_file->close = close_source;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_virtual_source_locator_init(
    struct kefir_preprocessor_virtual_source_locator *locator) {
    REQUIRE(locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to virtual source locator"));

    REQUIRE_OK(kefir_hashtree_init(&locator->sources, &kefir_hashtree_str_ops));
    locator->locator.payload = &locator;
    locator->locator.open = open_source;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_virtual_source_locator_free(
    struct kefir_mem *mem, struct kefir_preprocessor_virtual_source_locator *locator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(locator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid virtual source locator"));

    REQUIRE_OK(kefir_hashtree_free(mem, &locator->sources));
    locator->locator.open = NULL;
    locator->locator.payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_virtual_source_locator_register(
    struct kefir_mem *mem, struct kefir_preprocessor_virtual_source_locator *locator, const char *filepath,
    const char *content) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(locator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid virtual source locator"));
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid file path"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content"));

    REQUIRE_OK(kefir_hashtree_insert(mem, &locator->sources, (kefir_hashtree_key_t) filepath,
                                     (kefir_hashtree_value_t) content));
    return KEFIR_OK;
}
