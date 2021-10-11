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

const char *getstr1() {
    static const char value[] = "Hello, cruel WORLD!";
    return value;
}

const char *getstr2() {
    static const char value[] = "";
    return value;
}

const char *getstr3() {
    static const char value[] = "Test...test...test...";
    return value;
}

const char *getstr4() {
    static const char value2[] = "\n\n\t\t   ";
    return value2;
}

const char *getstr5() {
    static const char value[] = "Some value\n\t ?";
    return value;
}
