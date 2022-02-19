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
    assert(GLOBAL_UNINIT_STATIC != NULL);
    assert(get_global_uninit_static() == GLOBAL_UNINIT_STATIC);
    assert(GLOBAL_UNINIT_STATIC->x == 0);
    assert(GLOBAL_UNINIT_STATIC->y == 0);
    assert(GLOBAL_UNINIT_STATIC->ptr == NULL);
    assert(get_local_uninit_static() != NULL);
    assert(get_local_uninit_static()->x == 0);
    assert(get_local_uninit_static()->y == 0);
    assert(get_local_uninit_static()->ptr == 0);
    return EXIT_SUCCESS;
}
