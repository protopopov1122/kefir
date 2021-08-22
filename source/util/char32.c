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

#include "kefir/util/char32.h"
#include "kefir/core/util.h"

kefir_size_t kefir_strlen32(const kefir_char32_t *string) {
    REQUIRE(string != NULL, 0);
    kefir_size_t length = 0;
    while (string[length] != U'\0') {
        length++;
    }
    return length;
}

kefir_bool_t kefir_isspace32(kefir_char32_t chr) {
    switch (chr) {
        case U' ':
        case U'\f':
        case U'\v':
        case U'\t':
        case U'\n':
        case U'\r':
            return true;

        default:
            return false;
    }
}

kefir_bool_t kefir_isdigit32(kefir_char32_t chr) {
    switch (chr) {
        case U'0':
        case U'1':
        case U'2':
        case U'3':
        case U'4':
        case U'5':
        case U'6':
        case U'7':
        case U'8':
        case U'9':
            return true;

        default:
            return false;
    }
}

kefir_bool_t kefir_isoctdigit32(kefir_char32_t chr) {
    switch (chr) {
        case U'0':
        case U'1':
        case U'2':
        case U'3':
        case U'4':
        case U'5':
        case U'6':
        case U'7':
            return true;

        default:
            return false;
    }
}

kefir_bool_t kefir_ishexdigit32(kefir_char32_t chr) {
    switch (chr) {
        case U'a':
        case U'A':
        case U'b':
        case U'B':
        case U'c':
        case U'C':
        case U'd':
        case U'D':
        case U'e':
        case U'E':
        case U'f':
        case U'F':
            return true;

        default:
            return kefir_isdigit32(chr);
    }
}

kefir_bool_t kefir_isnondigit32(kefir_char32_t chr) {
    switch (chr) {
        case U'_':
        case U'a':
        case U'b':
        case U'c':
        case U'd':
        case U'e':
        case U'f':
        case U'g':
        case U'h':
        case U'i':
        case U'j':
        case U'k':
        case U'l':
        case U'm':
        case U'n':
        case U'o':
        case U'p':
        case U'q':
        case U'r':
        case U's':
        case U't':
        case U'u':
        case U'v':
        case U'w':
        case U'x':
        case U'y':
        case U'z':
        case U'A':
        case U'B':
        case U'C':
        case U'D':
        case U'E':
        case U'F':
        case U'G':
        case U'H':
        case U'I':
        case U'J':
        case U'K':
        case U'L':
        case U'M':
        case U'N':
        case U'O':
        case U'P':
        case U'Q':
        case U'R':
        case U'S':
        case U'T':
        case U'U':
        case U'V':
        case U'W':
        case U'X':
        case U'Y':
        case U'Z':
            return true;

        default:
            return false;
    }
}

kefir_int_t kefir_strcmp32(const kefir_char32_t *string1, const kefir_char32_t *string2) {
    REQUIRE(string1 != NULL && string2 != NULL, KEFIR_STRCMP32_ERROR);
    for (; *string1 != U'\0' || *string2 != U'\0'; string1++, string2++) {
        if (*string1 > *string2) {
            return 1;
        } else if (*string1 < *string2) {
            return -1;
        }
    }

    // if (*string1 > *string2) {
    //     return 1;
    // } else if (*string1 < *string2) {
    //     return -1;
    // }
    return 0;
}

kefir_uint32_t kefir_hex32todec(kefir_char32_t chr) {
    switch (chr) {
        case U'a':
        case U'A':
            return 10;

        case U'b':
        case U'B':
            return 11;

        case U'c':
        case U'C':
            return 12;

        case U'd':
        case U'D':
            return 13;

        case U'e':
        case U'E':
            return 14;

        case U'f':
        case U'F':
            return 15;

        default:
            if (chr >= U'0' && chr <= U'9') {
                return chr - U'0';
            } else {
                return (kefir_uint32_t) -1;
            }
    }
}
