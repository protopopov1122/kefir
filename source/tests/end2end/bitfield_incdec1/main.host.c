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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "./definitions.h"

int main() {
    for (int i = -10; i < 10; i++) {
        struct Structure s = {.a = i, .b = i + 1, .c = i + 2, .d = i + 10, .e = i + 11};

        preinc_a(&s);
        preinc_b(&s);
        preinc_c(&s);
        preinc_d(&s);
        preinc_e(&s);

        assert(s.a == i + 1);
        assert(s.b == i + 2);
        assert(s.c == i + 3);
        assert(s.d == i + 11);
        assert(s.e == i + 12);

        postinc_a(&s);
        postinc_b(&s);
        postinc_c(&s);
        postinc_d(&s);
        postinc_e(&s);

        assert(s.a == i + 2);
        assert(s.b == i + 3);
        assert(s.c == i + 4);
        assert(s.d == i + 12);
        assert(s.e == i + 13);

        predec_a(&s);
        predec_b(&s);
        predec_c(&s);
        predec_d(&s);
        predec_e(&s);

        assert(s.a == i + 1);
        assert(s.b == i + 2);
        assert(s.c == i + 3);
        assert(s.d == i + 11);
        assert(s.e == i + 12);

        postdec_a(&s);
        postdec_b(&s);
        postdec_c(&s);
        postdec_d(&s);
        postdec_e(&s);

        assert(s.a == i);
        assert(s.b == i + 1);
        assert(s.c == i + 2);
        assert(s.d == i + 10);
        assert(s.e == i + 11);
    }
    return EXIT_SUCCESS;
}
