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

#include "kefir/core/os_error.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

kefir_result_t kefir_set_os_error(const char *message, const char *file, unsigned int line,
                                  struct kefir_error **error_ptr) {
    struct kefir_error *error = NULL;
    kefir_result_t res = kefir_set_error(KEFIR_OS_ERROR, strerror(errno), file, line, &error);
    if (error == NULL) {
        return res;
    }

    if (message != NULL) {
        snprintf(error->payload, KEFIR_ERROR_PAYLOAD_LENGTH, "%s (%s)", message, error->message);
        error->message = error->payload;
    }
    if (error_ptr != NULL) {
        *error_ptr = error;
    }
    return res;
}
