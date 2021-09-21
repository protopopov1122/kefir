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

#include "kefir/preprocessor/token_sequence.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct buffer_element {
    struct kefir_token_buffer buffer;
    kefir_size_t index;
};

static kefir_result_t free_token_buffer(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                        void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_token_buffer *, buffer, entry->value);

    REQUIRE_OK(kefir_token_buffer_free(mem, buffer));
    KEFIR_FREE(mem, buffer);
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_sequence_init(struct kefir_preprocessor_token_sequence *seq) {
    REQUIRE(seq != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to token sequence"));

    REQUIRE_OK(kefir_list_init(&seq->buffer_stack));
    REQUIRE_OK(kefir_list_on_remove(&seq->buffer_stack, free_token_buffer, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_sequence_free(struct kefir_mem *mem,
                                                      struct kefir_preprocessor_token_sequence *seq) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(seq != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token sequence"));

    REQUIRE_OK(kefir_list_free(mem, &seq->buffer_stack));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_sequence_push_front(struct kefir_mem *mem,
                                                            struct kefir_preprocessor_token_sequence *seq,
                                                            struct kefir_token_buffer *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(seq != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token sequence"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    struct buffer_element *buffer_elt = KEFIR_MALLOC(mem, sizeof(struct buffer_element));
    REQUIRE(buffer_elt != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate token buffer element"));
    kefir_result_t res = kefir_token_buffer_init(&buffer_elt->buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, buffer_elt);
        return res;
    });
    res = kefir_token_buffer_insert(mem, &buffer_elt->buffer, src);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &buffer_elt->buffer);
        KEFIR_FREE(mem, buffer_elt);
        return res;
    });
    buffer_elt->index = 0;

    res = kefir_list_insert_after(mem, &seq->buffer_stack, kefir_list_tail(&seq->buffer_stack), buffer_elt);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &buffer_elt->buffer);
        KEFIR_FREE(mem, buffer_elt);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_buffer_elt(struct kefir_mem *mem, struct kefir_preprocessor_token_sequence *seq,
                                      struct buffer_element **buffer_elt) {
    *buffer_elt = NULL;
    while (*buffer_elt == NULL) {
        struct kefir_list_entry *entry = kefir_list_tail(&seq->buffer_stack);
        REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_ITERATOR_END, "Token sequence is empty"));
        *buffer_elt = entry->value;
        if ((*buffer_elt)->index == (*buffer_elt)->buffer.length) {
            REQUIRE_OK(kefir_list_pop(mem, &seq->buffer_stack, entry));
            *buffer_elt = NULL;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_sequence_next(struct kefir_mem *mem,
                                                      struct kefir_preprocessor_token_sequence *seq,
                                                      struct kefir_token *token_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(seq != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token sequence"));

    struct buffer_element *buffer_elt = NULL;
    REQUIRE_OK(next_buffer_elt(mem, seq, &buffer_elt));
    if (token_ptr != NULL) {
        REQUIRE_OK(kefir_token_move(token_ptr, &buffer_elt->buffer.tokens[buffer_elt->index]));
    }
    buffer_elt->index++;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_sequence_shift(struct kefir_mem *mem,
                                                       struct kefir_preprocessor_token_sequence *seq,
                                                       struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(seq != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token sequence"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    struct kefir_token token;
    REQUIRE_OK(kefir_preprocessor_token_sequence_next(mem, seq, &token));
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_token_sequence_current(struct kefir_mem *mem,
                                                         struct kefir_preprocessor_token_sequence *seq,
                                                         const struct kefir_token **token_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(seq != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token sequence"));
    REQUIRE(token_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to token"));

    struct buffer_element *buffer_elt = NULL;
    REQUIRE_OK(next_buffer_elt(mem, seq, &buffer_elt));
    *token_ptr = &buffer_elt->buffer.tokens[buffer_elt->index];
    return KEFIR_OK;
}
