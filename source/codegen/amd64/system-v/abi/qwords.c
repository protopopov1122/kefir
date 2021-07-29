/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/codegen/amd64/system-v/abi/qwords.h"
#include "kefir/core/error.h"

struct qword_counter {
    const struct kefir_vector *layout;
    kefir_size_t count;
};

static kefir_result_t count_qwords_visitor(const struct kefir_ir_type *type, kefir_size_t index,
                                           const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct qword_counter *counter = (struct qword_counter *) payload;
    ASSIGN_DECL_CAST(const struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(counter->layout, index));
    kefir_size_t count = layout->size / KEFIR_AMD64_SYSV_ABI_QWORD;
    if (layout->size % KEFIR_AMD64_SYSV_ABI_QWORD != 0) {
        count++;
    }
    counter->count += count;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_count(const struct kefir_ir_type *type, const struct kefir_vector *layout,
                                                 kefir_size_t *count) {
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, count_qwords_visitor);
    struct qword_counter counter = {.layout = layout, .count = 0};
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, &visitor, (void *) &counter, 0, kefir_ir_type_nodes(type)));
    *count = counter.count;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_alloc(struct kefir_amd64_sysv_abi_qwords *qwords, struct kefir_mem *mem,
                                                 kefir_size_t qword_count) {
    REQUIRE(qwords != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord vector"));
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(qword_count > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero QWord count"));
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_amd64_sysv_abi_qword), qword_count, &qwords->qwords));
    kefir_vector_extend(&qwords->qwords, qword_count);
    for (kefir_size_t i = 0; i < qword_count; i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&qwords->qwords, i));
        qword->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
        qword->location = 0;
        qword->index = i;
        qword->current_offset = 0;
    }
    qwords->current = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_free(struct kefir_amd64_sysv_abi_qwords *qwords, struct kefir_mem *mem) {
    REQUIRE(qwords != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord vector"));
    return kefir_vector_free(mem, &qwords->qwords);
}

static kefir_amd64_sysv_data_class_t derive_dataclass(kefir_amd64_sysv_data_class_t first,
                                                      kefir_amd64_sysv_data_class_t second) {
    if (first == second) {
        return first;
    }
    if (first == KEFIR_AMD64_SYSV_PARAM_NO_CLASS) {
        return second;
    }
    if (second == KEFIR_AMD64_SYSV_PARAM_NO_CLASS) {
        return first;
    }
#define ANY_OF(x, y, a) ((x) == (a) || (y) == (a))
    if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_MEMORY)) {
        return KEFIR_AMD64_SYSV_PARAM_MEMORY;
    }
    if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_INTEGER)) {
        return KEFIR_AMD64_SYSV_PARAM_INTEGER;
    }
    if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_X87) || ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_X87UP) ||
        ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_COMPLEX_X87)) {
        return KEFIR_AMD64_SYSV_PARAM_MEMORY;
    }
#undef ANY_OF
    return KEFIR_AMD64_SYSV_PARAM_SSE;
}

struct kefir_amd64_sysv_abi_qword *next_qword(struct kefir_amd64_sysv_abi_qwords *qwords, kefir_size_t alignment) {
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&qwords->qwords, qwords->current));
    kefir_size_t unalign = qword->current_offset % alignment;
    kefir_size_t pad = unalign > 0 ? alignment - unalign : 0;
    if (alignment == 0 || qword->current_offset + pad >= KEFIR_AMD64_SYSV_ABI_QWORD) {
        qwords->current++;
        qword = (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, qwords->current);
    } else {
        qword->current_offset += pad;
    }
    return qword;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_next(struct kefir_amd64_sysv_abi_qwords *qwords,
                                                kefir_amd64_sysv_data_class_t dataclass, kefir_size_t size,
                                                kefir_size_t alignment, struct kefir_amd64_sysv_abi_qword_ref *ref) {
    REQUIRE(qwords != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord vector"));
    REQUIRE(size > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero data size"));
    REQUIRE(ref != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord reference"));
    struct kefir_amd64_sysv_abi_qword *first = NULL;
    while (size > 0) {
        struct kefir_amd64_sysv_abi_qword *current = next_qword(qwords, alignment);
        if (first == NULL) {
            first = current;
            ref->qword = current;
            ref->offset = current->current_offset;
        }
        kefir_size_t available = KEFIR_AMD64_SYSV_ABI_QWORD - current->current_offset;
        kefir_size_t chunk = MIN(available, size);
        current->current_offset += chunk;
        size -= chunk;
        current->klass = derive_dataclass(current->klass, dataclass);
        alignment = 1;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_reset_class(struct kefir_amd64_sysv_abi_qwords *qwords,
                                                       kefir_amd64_sysv_data_class_t dataclass, kefir_size_t begin,
                                                       kefir_size_t count) {
    REQUIRE(qwords != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord vector"));
    const kefir_size_t length = kefir_vector_length(&qwords->qwords);
    REQUIRE(begin < length, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Index exceeds QWord vector length"));
    REQUIRE(count > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero number of QWords"));
    for (kefir_size_t i = begin; i < MIN(length, begin + count); i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&qwords->qwords, i));
        qword->klass = dataclass;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_save_position(const struct kefir_amd64_sysv_abi_qwords *qwords,
                                                         struct kefir_amd64_sysv_abi_qword_position *position) {
    REQUIRE(qwords != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord vector"));
    REQUIRE(position != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord position"));
    position->index = qwords->current;
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&qwords->qwords, qwords->current));
    position->offset = qword->current_offset;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_restore_position(
    struct kefir_amd64_sysv_abi_qwords *qwords, const struct kefir_amd64_sysv_abi_qword_position *position) {
    REQUIRE(qwords != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord vector"));
    REQUIRE(position != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid QWord position"));
    const kefir_size_t length = kefir_vector_length(&qwords->qwords);
    REQUIRE(position->index <= length,
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Position index exceeds QWord vector length"));
    REQUIRE((position->offset <= 8 && position->index < length) || (position->offset == 0 && position->index == length),
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Position offset exceeds boundaries"));
    for (kefir_size_t i = position->index; i < length; i++) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_abi_qword *, qword, kefir_vector_at(&qwords->qwords, i));
        qword->current_offset = i == position->index ? position->offset : position->index;
    }
    qwords->current = position->index;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_max_position(const struct kefir_amd64_sysv_abi_qword_position *first,
                                                        const struct kefir_amd64_sysv_abi_qword_position *second,
                                                        struct kefir_amd64_sysv_abi_qword_position *result) {
    REQUIRE(first != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid first position argument"));
    REQUIRE(second != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid second position argument"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid result pointer"));
    if (first->index > second->index || (first->index == second->index && first->offset >= second->offset)) {
        *result = *first;
    } else {
        *result = *second;
    }
    return KEFIR_OK;
}
