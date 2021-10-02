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

void wstring_uppercase(wchar_t *string) {
    for (; *string != L'\0'; ++string) {
        switch (*string) {
            case L'a':
                *string = L'A';
                break;
            case L'b':
                *string = L'B';
                break;
            case L'c':
                *string = L'C';
                break;
            case L'd':
                *string = L'D';
                break;
            case L'e':
                *string = L'E';
                break;
            case L'f':
                *string = L'F';
                break;
            case L'g':
                *string = L'G';
                break;
            case L'h':
                *string = L'H';
                break;
            case L'i':
                *string = L'I';
                break;
            case L'j':
                *string = L'J';
                break;
            case L'k':
                *string = L'K';
                break;
            case L'l':
                *string = L'L';
                break;
            case L'm':
                *string = L'M';
                break;
            case L'n':
                *string = L'N';
                break;
            case L'o':
                *string = L'O';
                break;
            case L'p':
                *string = L'P';
                break;
            case L'q':
                *string = L'Q';
                break;
            case L'r':
                *string = L'R';
                break;
            case L's':
                *string = L'S';
                break;
            case L't':
                *string = L'T';
                break;
            case L'u':
                *string = L'U';
                break;
            case L'v':
                *string = L'V';
                break;
            case L'w':
                *string = L'W';
                break;
            case L'x':
                *string = L'X';
                break;
            case L'y':
                *string = L'Y';
                break;
            case L'z':
                *string = L'Z';
                break;

            default:
                break;
        }
    }
}