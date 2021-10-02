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
#include "./definitions.h"

void u16string_uppercase(char16_t string[]) {
    for (; *string != u'\0'; ++string) {
        switch (*string) {
            case u'a':
                *string = u'A';
                break;
            case u'b':
                *string = u'B';
                break;
            case u'c':
                *string = u'C';
                break;
            case u'd':
                *string = u'D';
                break;
            case u'e':
                *string = u'E';
                break;
            case u'f':
                *string = u'F';
                break;
            case u'g':
                *string = u'G';
                break;
            case u'h':
                *string = u'H';
                break;
            case u'i':
                *string = u'I';
                break;
            case u'j':
                *string = u'J';
                break;
            case u'k':
                *string = u'K';
                break;
            case u'l':
                *string = u'L';
                break;
            case u'm':
                *string = u'M';
                break;
            case u'n':
                *string = u'N';
                break;
            case u'o':
                *string = u'O';
                break;
            case u'p':
                *string = u'P';
                break;
            case u'q':
                *string = u'Q';
                break;
            case u'r':
                *string = u'R';
                break;
            case u's':
                *string = u'S';
                break;
            case u't':
                *string = u'T';
                break;
            case u'u':
                *string = u'U';
                break;
            case u'v':
                *string = u'V';
                break;
            case u'w':
                *string = u'W';
                break;
            case u'x':
                *string = u'X';
                break;
            case u'y':
                *string = u'Y';
                break;
            case u'z':
                *string = u'Z';
                break;

            default:
                break;
        }
    }
}