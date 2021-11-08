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

int test_vla(unsigned long count, unsigned long sz) {
    void *ptr = 0;
    while (1) {
        char buf[sz];
        while (1) {
            while (1) {
                char buf2[sz];
                if (count-- == 0) {
                    goto end_of_outer_loop;
                } else {
                    break;
                }
            }
            char array[sz];
            if (ptr != 0 && ptr != array) {
                return 0;
            }
            ptr = array;
            goto end_of_inner_loop;
        }
    end_of_inner_loop:
        continue;
    }
end_of_outer_loop:
    return 1;
}
