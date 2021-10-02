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

#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "./definitions.h"

int main() {
    time_t expected_timestamp = 1633204489;
    struct tm *time = localtime(&expected_timestamp);
    char buffer[256] = {0};
    strftime(buffer, sizeof(buffer) - 1, "%b %e %Y %H:%M:%S", time);
    assert(strcmp(get_build_timestamp(), buffer) == 0);
    return EXIT_SUCCESS;
}
