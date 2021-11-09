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

#ifndef KEFIR_CORE_EXTENSIONS_H_
#define KEFIR_CORE_EXTENSIONS_H_

#define KEFIR_RUN_EXTENSION(_res, _mem, _object, _ext, ...)                         \
    do {                                                                            \
        if ((_object)->extensions != NULL && (_object)->extensions->_ext != NULL) { \
            *(_res) = (_object)->extensions->_ext((_mem), (_object), __VA_ARGS__);  \
        } else {                                                                    \
            *(_res) = KEFIR_OK;                                                     \
        }                                                                           \
    } while (0);

#define KEFIR_RUN_EXTENSION0(_res, _mem, _object, _ext)                             \
    do {                                                                            \
        if ((_object)->extensions != NULL && (_object)->extensions->_ext != NULL) { \
            *(_res) = (_object)->extensions->_ext((_mem), (_object));               \
        } else {                                                                    \
            *(_res) = KEFIR_OK;                                                     \
        }                                                                           \
    } while (0);

#endif
