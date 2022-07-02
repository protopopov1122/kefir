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

#ifndef __OpenBSD__

_Thread_local long counter = 0;
extern _Thread_local long external_counter;
static _Thread_local long saved_counter;

long get_counter() {
    return counter;
}

void advance_counter() {
    counter++;
}

void import_counter() {
    counter = external_counter;
}

void save_counter() {
    saved_counter = counter;
}

void restore_counter() {
    counter = saved_counter;
}

int reset_counter() {
    static _Thread_local int resets = 0;
    counter = 0;
    return ++resets;
}

#endif