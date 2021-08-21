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

#include "kefir/core/string_buffer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>
#include <uchar.h>

#define BUFFER_MIN_CAPACITY 32
#define BUFFER_GROW 64

kefir_result_t kefir_string_buffer_init(struct kefir_mem *mem, struct kefir_string_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string buffer"));

    buffer->capacity = BUFFER_MIN_CAPACITY;
    buffer->length = 0;
    buffer->buffer = KEFIR_MALLOC(mem, buffer->capacity);
    REQUIRE(buffer->buffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate string buffer"));
    memset(buffer->buffer, 0, buffer->capacity);
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_free(struct kefir_mem *mem, struct kefir_string_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string buffer"));

    KEFIR_FREE(mem, buffer->buffer);
    buffer->buffer = NULL;
    buffer->capacity = 0;
    buffer->length = 0;
    return KEFIR_OK;
}

const char *kefir_string_buffer_value(const struct kefir_string_buffer *buffer, kefir_size_t *length_ptr) {
    if (buffer == NULL) {
        ASSIGN_PTR(length_ptr, 0);
        return NULL;
    } else {
        ASSIGN_PTR(length_ptr, buffer->length + 1);
        return buffer->buffer;
    }
}

static kefir_size_t ensure_capacity(struct kefir_mem *mem, struct kefir_string_buffer *buffer, kefir_size_t size) {
    if (buffer->length + size + 1 >= buffer->capacity) {
        kefir_size_t newCapacity = buffer->length + size + 1 + BUFFER_GROW;
        char *newBuffer = KEFIR_MALLOC(mem, newCapacity);
        REQUIRE(newBuffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to reallocate string buffer"));
        memset(newBuffer, 0, newCapacity);
        memcpy(newBuffer, buffer->buffer, buffer->length);
        KEFIR_FREE(mem, buffer->buffer);
        buffer->buffer = newBuffer;
        buffer->capacity = newCapacity;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_insert(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                          const char *fragment, kefir_size_t size) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string buffer"));
    REQUIRE(fragment != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string fragment"));
    REQUIRE(size != 0, KEFIR_OK);

    REQUIRE_OK(ensure_capacity(mem, buffer, size));
    memcpy(buffer->buffer + buffer->length, fragment, size);
    buffer->length += size;
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_insert32(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                            kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string buffer"));

    char narrow_string[MB_CUR_MAX];
    mbstate_t mbstate = {0};
    size_t sz = c32rtomb(narrow_string, character, &mbstate);
    REQUIRE(sz != (size_t) -1,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Failed to convert wide character into multi-byte string"));
    REQUIRE_OK(kefir_string_buffer_insert(mem, buffer, narrow_string, sz));
    return KEFIR_OK;
}
