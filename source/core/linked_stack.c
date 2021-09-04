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

#include "kefir/core/linked_stack.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_linked_stack_push(struct kefir_mem *mem, struct kefir_list *list, void *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list"));
    REQUIRE_OK(kefir_list_insert_after(mem, list, kefir_list_tail(list), value));
    return KEFIR_OK;
}

kefir_result_t kefir_linked_stack_pop(struct kefir_mem *mem, struct kefir_list *list, void **value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list"));
    if (value != NULL) {
        *value = NULL;
    }
    struct kefir_list_entry *tail = kefir_list_tail(list);
    if (tail != NULL) {
        if (value != NULL) {
            *value = tail->value;
        }
        REQUIRE_OK(kefir_list_pop(mem, list, tail));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_linked_stack_peek(const struct kefir_list *list, void **value) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid value pointer"));
    *value = NULL;
    struct kefir_list_entry *tail = kefir_list_tail(list);
    if (tail != NULL) {
        *value = tail->value;
    }
    return KEFIR_OK;
}
