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

void *get_a(struct S *s) {
    return &s->a;
}

void *get_b(struct S *s) {
    return &s->b;
}

void *get_c(struct S *s) {
    return &s->c;
}

void *get_d(struct S *s) {
    return &s->d;
}

void *get_x(struct S *s) {
    return &s->d.x;
}

void *get_y(struct S *s) {
    return &s->d.y;
}

void *get_z(struct S *s) {
    return &s->d.z;
}

void *get_Z(struct S *s) {
    return &s->d.Z;
}
