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

#ifndef KEFIR_CORE_OPTIONAL_H_
#define KEFIR_CORE_OPTIONAL_H_

#include <string.h>
#include "kefir/core/basic-types.h"

#define KEFIR_OPTIONAL_TYPE(_type) \
    struct {                       \
        kefir_bool_t empty;        \
        _type value;               \
    }

#define KEFIR_NAMED_OPTIONAL_TYPE(_name, _type) \
    struct _name {                              \
        kefir_bool_t empty;                     \
        _type value;                            \
    }

#define KEFIR_OPTIONAL_EMPTY(_opt) ((_opt)->empty)

#define KEFIR_OPTIONAL_VALUE(_opt) ((_opt)->empty ? NULL : &(_opt)->value)

#define KEFIR_OPTIONAL_SET_EMPTY(_opt)                    \
    do {                                                  \
        (_opt)->empty = true;                             \
        memset(&(_opt)->value, 0, sizeof((_opt)->value)); \
    } while (0)

#define KEFIR_OPTIONAL_SET_VALUE(_opt, _value) \
    do {                                       \
        (_opt)->empty = false;                 \
        (_opt)->value = (_value);              \
    } while (0)

#endif
