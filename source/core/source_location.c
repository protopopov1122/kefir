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

#include "kefir/core/source_location.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_source_location_empty(struct kefir_source_location *location) {
    REQUIRE(location != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to source location"));

    location->source = NULL;
    location->line = 0;
    location->column = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_source_location_init(struct kefir_source_location *location, const char *source,
                                          kefir_source_location_line_t line, kefir_source_location_column_t column) {
    REQUIRE(location != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to source location"));
    REQUIRE(source != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid source"));

    location->source = source;
    location->line = line;
    location->column = column;
    return KEFIR_OK;
}

kefir_bool_t kefir_source_location_get(const struct kefir_source_location *location, const char **source_ptr,
                                       kefir_source_location_line_t *line_ptr,
                                       kefir_source_location_column_t *column_ptr) {
    REQUIRE(location != NULL, false);
    if (location->source != NULL) {
        ASSIGN_PTR(source_ptr, location->source);
        ASSIGN_PTR(line_ptr, location->line);
        ASSIGN_PTR(column_ptr, location->column);
        return true;
    } else {
        return false;
    }
}
