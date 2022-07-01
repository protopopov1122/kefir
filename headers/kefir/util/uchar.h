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

#ifndef KEFIR_UTIL_UCHAR_H_
#define KEFIR_UTIL_UCHAR_H_

#include "kefir/core/platform.h"

#ifndef KEFIR_OPENBSD_HOST_PLATFORM
#include <uchar.h>
#else
typedef __uint_least16_t char16_t;
typedef __uint_least32_t char32_t;

size_t c16rtomb(char *__restrict, char16_t, mbstate_t *__restrict);
size_t mbrtoc16(char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

size_t c32rtomb(char *__restrict, char32_t, mbstate_t *__restrict);
size_t mbrtoc32(char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

#endif
#endif
