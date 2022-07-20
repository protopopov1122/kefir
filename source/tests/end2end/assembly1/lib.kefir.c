/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

// Assembly directives do not exist beyond parsing stage,
// thus semantically contain garbage

asm volatile goto("Test...Testt...\n"
                  " test...\n"
                  : [output] "r"(*temp), "r"(*temp[2])
                  : "w"(some_input + 1), [test] "w+"(some_other_input())
                  : "clobber1", "clobber2"
                  : label1, label2, label3);

const int Value1 = 10203010;

asm("Some other assembly");

int function1(int x) {
    asm inline("Another assembly"
               "directive"
               : "w"(x)
               : "r"(x));
    int y = ~x;
    asm("And once more assembly" : [variable] "r"(y));
    return y + 1;
}
