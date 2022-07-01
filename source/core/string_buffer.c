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
#include "kefir/util/uchar.h"
#include <string.h>

#define BUFFER_MIN_CAPACITY 32
#define BUFFER_GROW 64

kefir_result_t kefir_string_buffer_init(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                        kefir_string_buffer_mode_t mode) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));

    buffer->mode = mode;
    buffer->capacity = BUFFER_MIN_CAPACITY;
    buffer->length = 0;
    buffer->buffer = KEFIR_MALLOC(mem, buffer->capacity);
    REQUIRE(buffer->buffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate string buffer"));
    memset(buffer->buffer, 0, buffer->capacity);
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_init_value(struct kefir_string_buffer *buffer, kefir_string_buffer_mode_t mode,
                                              void *value, kefir_size_t length) {
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer value"));

    buffer->mode = mode;
    buffer->buffer = value;

    switch (mode) {
        case KEFIR_STRING_BUFFER_MULTIBYTE:
        case KEFIR_STRING_BUFFER_UNICODE8:
            buffer->length = length - 1;
            buffer->capacity = length;
            break;

        case KEFIR_STRING_BUFFER_UNICODE16:
            buffer->length = (length - 1) * sizeof(kefir_char16_t);
            buffer->capacity = length * sizeof(kefir_char16_t);
            break;

        case KEFIR_STRING_BUFFER_UNICODE32:
            buffer->length = (length - 1) * sizeof(kefir_char32_t);
            buffer->capacity = length * sizeof(kefir_char32_t);
            break;

        case KEFIR_STRING_BUFFER_WIDE:
            buffer->length = (length - 1) * sizeof(kefir_wchar_t);
            buffer->capacity = length * sizeof(kefir_wchar_t);
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_free(struct kefir_mem *mem, struct kefir_string_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));

    KEFIR_FREE(mem, buffer->buffer);
    buffer->buffer = NULL;
    buffer->capacity = 0;
    buffer->length = 0;
    return KEFIR_OK;
}

const void *kefir_string_buffer_value(const struct kefir_string_buffer *buffer, kefir_size_t *length_ptr) {
    if (buffer == NULL) {
        ASSIGN_PTR(length_ptr, 0);
        return NULL;
    } else {
        switch (buffer->mode) {
            case KEFIR_STRING_BUFFER_MULTIBYTE:
            case KEFIR_STRING_BUFFER_UNICODE8:
                ASSIGN_PTR(length_ptr, buffer->length + 1);
                break;

            case KEFIR_STRING_BUFFER_UNICODE16:
                ASSIGN_PTR(length_ptr, buffer->length / sizeof(kefir_char16_t) + 1);
                break;

            case KEFIR_STRING_BUFFER_UNICODE32:
                ASSIGN_PTR(length_ptr, buffer->length / sizeof(kefir_char32_t) + 1);
                break;

            case KEFIR_STRING_BUFFER_WIDE:
                ASSIGN_PTR(length_ptr, buffer->length / sizeof(kefir_wchar_t) + 1);
                break;
        }
        return buffer->buffer;
    }
}

static kefir_size_t ensure_capacity(struct kefir_mem *mem, struct kefir_string_buffer *buffer, kefir_size_t size) {
    if (buffer->length + size + 1 >= buffer->capacity) {
        static const kefir_size_t NullBytes[] = {[KEFIR_STRING_BUFFER_MULTIBYTE] = 1,
                                                 [KEFIR_STRING_BUFFER_UNICODE8] = 1,
                                                 [KEFIR_STRING_BUFFER_UNICODE16] = sizeof(kefir_char16_t),
                                                 [KEFIR_STRING_BUFFER_UNICODE32] = sizeof(kefir_char32_t),
                                                 [KEFIR_STRING_BUFFER_WIDE] = sizeof(kefir_wchar_t)};
        kefir_size_t newCapacity = buffer->length + size + NullBytes[buffer->mode] + BUFFER_GROW;
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

static kefir_result_t insert_buffer(struct kefir_mem *mem, struct kefir_string_buffer *buffer, const void *fragment,
                                    kefir_size_t size) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(fragment != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string fragment"));
    REQUIRE(size != 0, KEFIR_OK);

    REQUIRE_OK(ensure_capacity(mem, buffer, size));
    memcpy(((char *) buffer->buffer) + buffer->length, fragment, size);
    buffer->length += size;
    return KEFIR_OK;
}

static kefir_result_t convert_to_multibyte(kefir_char32_t character, char *narrow_string, size_t *sz) {
    mbstate_t mbstate = {0};
    *sz = c32rtomb(narrow_string, character, &mbstate);
    REQUIRE(*sz != (size_t) -1,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert unicode32 character into multi-byte string"));
    return KEFIR_OK;
}

static kefir_result_t kefir_string_buffer_insert_multibyte(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                                           kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(buffer->mode == KEFIR_STRING_BUFFER_MULTIBYTE,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected mulibyte string buffer"));

    char narrow_string[MB_LEN_MAX];
    size_t sz;
    REQUIRE_OK(convert_to_multibyte(character, narrow_string, &sz));
    REQUIRE(sz != (size_t) -1,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert unicode32 character into multi-byte string"));
    REQUIRE_OK(insert_buffer(mem, buffer, narrow_string, sz));
    return KEFIR_OK;
}

static kefir_result_t kefir_string_buffer_insert_unicode8_character(struct kefir_mem *mem,
                                                                    struct kefir_string_buffer *buffer,
                                                                    kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(buffer->mode == KEFIR_STRING_BUFFER_UNICODE8,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected unicode8 string buffer"));

    char narrow_string[MB_LEN_MAX];
    size_t sz;
    REQUIRE_OK(convert_to_multibyte(character, narrow_string, &sz));
    REQUIRE(sz != (size_t) -1,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert unicode32 character into unicode8 string"));
    REQUIRE_OK(insert_buffer(mem, buffer, narrow_string, sz));
    return KEFIR_OK;
}

static kefir_result_t kefir_string_buffer_insert_unicode16_character(struct kefir_mem *mem,
                                                                     struct kefir_string_buffer *buffer,
                                                                     kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(buffer->mode == KEFIR_STRING_BUFFER_UNICODE16,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected unicode16 string buffer"));

    char narrow_string[MB_LEN_MAX];
    size_t sz;
    REQUIRE_OK(convert_to_multibyte(character, narrow_string, &sz));
    REQUIRE(sz != (size_t) -1,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert unicode32 character into unicode16 string"));

    mbstate_t mbstate = {0};
    const char *begin = narrow_string;
    const char *end = narrow_string + sz;
    union {
        kefir_char16_t value;
        const char bytes[sizeof(kefir_char16_t)];
    } u16chr;

    while (begin < end) {
        sz = mbrtoc16(&u16chr.value, begin, end - begin, &mbstate);
        switch (sz) {
            case (size_t) -1:
            case (size_t) -2:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE,
                                       "Failed to convert unicode32 character into unicode16 string");

            case (size_t) -3:
                REQUIRE_OK(insert_buffer(mem, buffer, u16chr.bytes, sizeof(kefir_char16_t)));
                break;

            case 0:
                REQUIRE_OK(insert_buffer(mem, buffer, u16chr.bytes, sizeof(kefir_char16_t)));
                begin = end;
                break;

            default:
                REQUIRE_OK(insert_buffer(mem, buffer, u16chr.bytes, sizeof(kefir_char16_t)));
                begin += sz;
                break;
        }
    }
    return KEFIR_OK;
}

static kefir_result_t kefir_string_buffer_insert_unicode32_character(struct kefir_mem *mem,
                                                                     struct kefir_string_buffer *buffer,
                                                                     kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(buffer->mode == KEFIR_STRING_BUFFER_UNICODE32,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected unicode32 string buffer"));

    union {
        kefir_char32_t value;
        const char bytes[sizeof(kefir_char32_t)];
    } u32chr = {.value = character};
    REQUIRE_OK(insert_buffer(mem, buffer, u32chr.bytes, sizeof(kefir_char32_t)));
    return KEFIR_OK;
}

static kefir_result_t kefir_string_buffer_insert_wide_character(struct kefir_mem *mem,
                                                                struct kefir_string_buffer *buffer,
                                                                kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(buffer->mode == KEFIR_STRING_BUFFER_WIDE,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected wide string buffer"));

    char narrow_string[MB_LEN_MAX];
    size_t sz;
    REQUIRE_OK(convert_to_multibyte(character, narrow_string, &sz));
    REQUIRE(sz != (size_t) -1, KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert unicode32 character into wide"));

    const char *begin = narrow_string;
    const char *end = narrow_string + sz;
    union {
        kefir_wchar_t value;
        const char bytes[sizeof(kefir_wchar_t)];
    } wchr;

    while (begin < end) {
        int rc = mbtowc(&wchr.value, begin, end - begin);
        switch (rc) {
            case -1:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to convert unicode32 character into wide");
            case 0:
                REQUIRE_OK(insert_buffer(mem, buffer, wchr.bytes, sizeof(kefir_wchar_t)));
                begin = end;
                break;

            default:
                REQUIRE_OK(insert_buffer(mem, buffer, wchr.bytes, sizeof(kefir_wchar_t)));
                begin += rc;
                break;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_insert(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                          kefir_char32_t character) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));

    switch (buffer->mode) {
        case KEFIR_STRING_BUFFER_MULTIBYTE:
            REQUIRE_OK(kefir_string_buffer_insert_multibyte(mem, buffer, character));
            break;

        case KEFIR_STRING_BUFFER_UNICODE8:
            REQUIRE_OK(kefir_string_buffer_insert_unicode8_character(mem, buffer, character));
            break;

        case KEFIR_STRING_BUFFER_UNICODE16:
            REQUIRE_OK(kefir_string_buffer_insert_unicode16_character(mem, buffer, character));
            break;

        case KEFIR_STRING_BUFFER_UNICODE32:
            REQUIRE_OK(kefir_string_buffer_insert_unicode32_character(mem, buffer, character));
            break;

        case KEFIR_STRING_BUFFER_WIDE:
            REQUIRE_OK(kefir_string_buffer_insert_wide_character(mem, buffer, character));
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_insert_raw(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                              kefir_int64_t raw_value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));

    switch (buffer->mode) {
        case KEFIR_STRING_BUFFER_MULTIBYTE:
        case KEFIR_STRING_BUFFER_UNICODE8: {
            REQUIRE(raw_value <= KEFIR_UCHAR_MAX,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Maximum character value is exceeded"));
            char value = (char) raw_value;
            REQUIRE_OK(insert_buffer(mem, buffer, &value, 1));
        } break;

        case KEFIR_STRING_BUFFER_UNICODE16: {
            REQUIRE(raw_value <= KEFIR_CHAR16_MAX,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Maximum unicode16 character value is exceeded"));
            union {
                kefir_char16_t raw;
                char bytes[sizeof(kefir_char16_t)];
            } value = {.raw = (kefir_char16_t) raw_value};
            REQUIRE_OK(insert_buffer(mem, buffer, &value.bytes, sizeof(kefir_char16_t)));
        } break;

        case KEFIR_STRING_BUFFER_UNICODE32: {
            REQUIRE(raw_value <= KEFIR_CHAR32_MAX,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Maximum unicode32 character value is exceeded"));
            union {
                kefir_char32_t raw;
                char bytes[sizeof(kefir_char32_t)];
            } value = {.raw = (kefir_char32_t) raw_value};
            REQUIRE_OK(insert_buffer(mem, buffer, &value.bytes, sizeof(kefir_char32_t)));
        } break;

        case KEFIR_STRING_BUFFER_WIDE: {
            REQUIRE(raw_value <= KEFIR_WCHAR_MAX,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Maximum wide character value is exceeded"));
            union {
                kefir_wchar_t raw;
                char bytes[sizeof(kefir_wchar_t)];
            } value = {.raw = (kefir_wchar_t) raw_value};
            REQUIRE_OK(insert_buffer(mem, buffer, &value.bytes, sizeof(kefir_wchar_t)));
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t insert_multibyte(struct kefir_mem *mem, struct kefir_string_buffer *buffer, const char *begin,
                                       const char *end) {
    mbstate_t mbstate = {0};
    while (begin < end) {
        kefir_char32_t character;
        size_t sz = mbrtoc32(&character, begin, end - begin, &mbstate);
        switch (sz) {
            case (size_t) -1:
            case (size_t) -2:
            case (size_t) -3:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Failed to decode multibyte string");

            case 0:
                begin++;
                break;

            default:
                begin += sz;
                break;
        }
        REQUIRE_OK(kefir_string_buffer_insert(mem, buffer, character));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_convert(struct kefir_mem *mem, struct kefir_string_buffer *buffer,
                                           kefir_string_buffer_mode_t newMode) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid string buffer"));
    REQUIRE(buffer->mode == KEFIR_STRING_BUFFER_MULTIBYTE,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Can only convert multibyte buffers"));

    switch (newMode) {
        case KEFIR_STRING_BUFFER_MULTIBYTE:
            // Intentionally left blank
            break;

        case KEFIR_STRING_BUFFER_UNICODE8:
            buffer->mode = newMode;
            break;

        case KEFIR_STRING_BUFFER_UNICODE16:
        case KEFIR_STRING_BUFFER_UNICODE32:
        case KEFIR_STRING_BUFFER_WIDE: {
            char *oldBuffer = buffer->buffer;
            kefir_size_t oldLength = buffer->length;
            *buffer = (struct kefir_string_buffer){0};
            kefir_result_t res = kefir_string_buffer_init(mem, buffer, newMode);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_FREE(mem, oldBuffer);
                return res;
            });

            res = insert_multibyte(mem, buffer, oldBuffer, oldBuffer + oldLength);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_FREE(mem, oldBuffer);
                return res;
            });
            KEFIR_FREE(mem, oldBuffer);
        } break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_string_buffer_merge(struct kefir_mem *mem, struct kefir_string_buffer *dst,
                                         const struct kefir_string_buffer *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination string buffer"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source string buffer"));

    switch (src->mode) {
        case KEFIR_STRING_BUFFER_MULTIBYTE:
        case KEFIR_STRING_BUFFER_UNICODE8: {
            const char *begin = src->buffer;
            REQUIRE_OK(insert_multibyte(mem, dst, begin, begin + src->length));
        } break;

        case KEFIR_STRING_BUFFER_UNICODE16: {
            const kefir_char16_t *begin = src->buffer;
            char buf[MB_LEN_MAX];
            mbstate_t mbstate = {0};
            for (kefir_size_t i = 0; i < src->length / sizeof(kefir_char16_t); i++) {
                size_t rc = c16rtomb(buf, begin[i], &mbstate);
                switch (rc) {
                    case (size_t) -1:
                        return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Invalid unicode16 character");

                    case 0:
                        // Intentionally left blank
                        break;

                    default:
                        REQUIRE_OK(insert_multibyte(mem, dst, buf, buf + rc));
                        break;
                }
            }
        } break;

        case KEFIR_STRING_BUFFER_UNICODE32: {
            const kefir_char32_t *begin = src->buffer;
            for (kefir_size_t i = 0; i < src->length / sizeof(kefir_char32_t); i++) {
                REQUIRE_OK(kefir_string_buffer_insert(mem, dst, begin[i]));
            }
        } break;

        case KEFIR_STRING_BUFFER_WIDE: {
            const kefir_wchar_t *begin = src->buffer;
            char buf[MB_LEN_MAX];
            mbstate_t mbstate = {0};
            for (kefir_size_t i = 0; i < src->length / sizeof(kefir_wchar_t); i++) {
                size_t rc = wcrtomb(buf, begin[i], &mbstate);
                switch (rc) {
                    case (size_t) -1:
                        return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Invalid wide character");

                    default:
                        REQUIRE_OK(insert_multibyte(mem, dst, buf, buf + rc));
                        break;
                }
            }
        } break;
    }
    return KEFIR_OK;
}
