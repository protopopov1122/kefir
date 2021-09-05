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

#ifndef KEFIR_CORE_SOURCE_ERROR_H_
#define KEFIR_CORE_SOURCE_ERROR_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/error.h"
#include "kefir/core/source_location.h"
#include <stddef.h>

typedef struct kefir_source_error {
    struct kefir_source_location source_location;
    char message[];
} kefir_source_error_t;

#define KEFIR_SOURCE_ERROR_MESSAGE_LENGTH (KEFIR_ERROR_PAYLOAD_LENGTH - offsetof(struct kefir_source_error, message))

kefir_result_t kefir_set_source_error(kefir_result_t, const struct kefir_source_location *, const char *, const char *,
                                      unsigned int, struct kefir_error **);

#define KEFIR_SET_SOURCE_ERROR(_code, _location, _msg) \
    kefir_set_source_error((_code), (_location), (_msg), __FILE__, __LINE__, NULL)

#endif
