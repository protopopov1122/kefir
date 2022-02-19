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

#include "./definitions.h"

static struct Structure1 STRUCT1;
static _Thread_local struct Structure1 STRUCT2;

struct Structure1 *GLOBAL_UNINIT_STATIC = &STRUCT1;

struct Structure1 *get_global_uninit_static() {
    return &STRUCT1;
}

struct Structure1 *get_local_uninit_static() {
    static struct Structure1 STRUCT2;
    return &STRUCT2;
}

struct Structure1 *get_global_tlocal_uninit_static() {
    return &STRUCT2;
}

struct Structure1 *get_local_tlocal_uninit_static() {
    static _Thread_local struct Structure1 STRUCT2;
    return &STRUCT2;
}
