/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

size_t custom_strlen(const char[]);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    const char *STRINGS[] = {
        "", "\0", "1", "123", "\0 123", "\t\t\t  ", "\t\t\t  \0", "HELLO, WORLD!", "HELLO, brutal WoRlD!\n\0"};
    size_t STRINGS_LENGTH = sizeof(STRINGS) / sizeof(STRINGS[0]);

    for (size_t i = 0; i < STRINGS_LENGTH; i++) {
        ASSERT(strlen(STRINGS[i]) == custom_strlen(STRINGS[i]));
    }
    return EXIT_SUCCESS;
}
