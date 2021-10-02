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

#define INTERNAL
#include "definitions.h"

struct content init_content() {
    return (struct content){.literal1 = u"String literal"
                                        "#1"
                                        u".",
                            .literal2 = "S"
                                        "t"
                                        "r"
                                        "i"
                                        "ng"
                                        u" literal#"
                                        "2.",
                            .literal3 = "Stri"
                                        "ng literal#3"
                                        U".",
                            .literal4 = U"Strin"
                                        "g"
                                        " "
                                        "l"
                                        "i"
                                        U"ter"
                                        "al#"
                                        U"4.",
                            .literal5 = L"Str"
                                        "ing lite"
                                        L"ral#"
                                        "5"
                                        ".",
                            .literal6 = "String lit"
                                        L"eral#"
                                        L"6."};
}