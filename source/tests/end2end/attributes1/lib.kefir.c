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

__attribute__((test1)) int __attribute__((test2("string", 2))) long __attribute__((test3(100, "string", 3.12 + 1)))
INTEGER __attribute__((attr)) = 192739,
        __attribute__((test100)) INTEGER2 __attribute__((test2000)) = -282761;

struct __attribute__((STRUCTURE)) __attribute__((STRUCT("X", x))) Test {
    long x;
    long y;
} __attribute__((TAIL));

__attribute__((X)) long __attribute__((FN("sum"))) sum(struct Test __attribute__((param)) t) __attribute__((TAIL)) {
    goto body;
    return 0;
body:
    __attribute__((SOME_LABEL("body"))) __attribute__((SOME_LABEL));
    __attribute__((another_attribute));
    return t.x + t.y;
}

enum __attribute__((ENUMERATOR)) __attribute__((ENUMERATOR("enum", enumerator, -1))) Enum {
    ONE __attribute__((FIELD(1))) __attribute__((test)) = 1,
    TWO __attribute__((FIELD(2))) __attribute__((test)) = 2
};

long get_one() __attribute__((XYZ)) __attribute__((ABC(XYZ))) {
    return ONE;
}

__attribute__((ONE)) long __attribute__((TWO)) __attribute__((NONE(none))) get_two() {
    return TWO;
}
