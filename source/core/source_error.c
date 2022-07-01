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

#include "kefir/core/source_error.h"
#include <stdio.h>
#include <stdarg.h>

kefir_result_t kefir_set_source_error(kefir_result_t code, const struct kefir_source_location *location,
                                      const char *message, const char *file, unsigned int line,
                                      struct kefir_error **error_ptr) {
    struct kefir_error *error = NULL;
    kefir_result_t res = kefir_set_error(code, message, file, line, &error);
    if (error == NULL) {
        return res;
    }

    struct kefir_source_error *source_error = (struct kefir_source_error *) error->payload;
    error->payload_type = KEFIR_ERROR_PAYLOAD_SOURCE_LOCATION;
    if (location != NULL) {
        source_error->source_location = *location;
        snprintf(source_error->message, KEFIR_SOURCE_ERROR_MESSAGE_LENGTH, "%s@%u:%u %s", location->source,
                 location->line, location->column, message);
        error->message = source_error->message;
    } else {
        kefir_source_location_empty(&source_error->source_location);
    }
    if (error_ptr != NULL) {
        *error_ptr = error;
    }
    return res;
}

kefir_result_t kefir_set_source_errorf(kefir_result_t code, const struct kefir_source_location *location,
                                       const char *fmt, const char *file, unsigned int line,
                                       struct kefir_error **error_ptr, ...) {
    struct kefir_error *error = NULL;
    kefir_result_t res = kefir_set_source_error(code, location, fmt, file, line, &error);
    if (error == NULL) {
        return res;
    }

    va_list args;
    va_start(args, error_ptr);
    struct kefir_source_error *source_error = (struct kefir_source_error *) error->payload;

    char *message_buf = source_error->message;
    kefir_size_t message_max_len = KEFIR_SOURCE_ERROR_MESSAGE_LENGTH;
    if (location != NULL) {
        int diff = snprintf(source_error->message, KEFIR_SOURCE_ERROR_MESSAGE_LENGTH, "%s@%u:%u ", location->source,
                            location->line, location->column);
        message_buf += diff;
        message_max_len -= diff;
    }
    vsnprintf(message_buf, message_max_len, fmt, args);
    va_end(args);
    error->message = source_error->message;

    if (error_ptr != NULL) {
        *error_ptr = error;
    }
    return res;
}
