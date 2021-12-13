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

static int get_one(void) {
    return 1;
}

static int get_zero(void) {
    return 0;
}

static int get_neg(void) {
    return -1;
}

int (*getfn(enum FnType tp))(void) {
    switch (tp) {
        case FNONE:
            return get_one;

        case FNNEG:
            return get_neg;

        default:
            return get_zero;
    }
}
