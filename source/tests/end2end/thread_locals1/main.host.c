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

#include <stdlib.h>
#ifndef __OpenBSD__
#include <stdio.h>
#include <assert.h>
#include <threads.h>
#include "./definitions.h"

_Thread_local long external_counter = 0;

static int thread2(void *payload) {
    (void) payload;

    assert(external_counter == 0);
    assert(get_counter() == 0);
    restore_counter();
    assert(get_counter() == 0);

    advance_counter();
    advance_counter();
    advance_counter();
    advance_counter();
    advance_counter();
    assert(get_counter() == 5);
    save_counter();
    assert(reset_counter() == 1);
    assert(get_counter() == 0);
    advance_counter();
    assert(get_counter() == 1);
    assert(reset_counter() == 2);
    assert(get_counter() == 0);

    external_counter = 119;
    import_counter();
    assert(get_counter() == 119);
    advance_counter();
    advance_counter();
    advance_counter();
    advance_counter();
    return (int) get_counter();
}

int main() {
    assert(get_counter() == 0);
    advance_counter();
    assert(get_counter() == 1);
    save_counter();
    advance_counter();
    advance_counter();
    assert(get_counter() == 3);
    restore_counter();
    assert(get_counter() == 1);
    advance_counter();
    advance_counter();
    advance_counter();
    advance_counter();
    assert(get_counter() == 5);
    external_counter = 100;
    import_counter();
    assert(get_counter() == 100);
    advance_counter();
    advance_counter();
    assert(get_counter() == 102);
    assert(reset_counter() == 1);
    assert(get_counter() == 0);
    advance_counter();
    advance_counter();
    restore_counter();
    assert(get_counter() == 1);
    assert(reset_counter() == 2);
    assert(reset_counter() == 3);
    assert(reset_counter() == 4);
    assert(get_counter() == 0);
    external_counter = 42;
    import_counter();
    save_counter();
    assert(get_counter() == 42);
    advance_counter();
    assert(get_counter() == 43);

    thrd_t thread;
    assert(thrd_create(&thread, thread2, NULL) == thrd_success);
    int res = 0;
    assert(thrd_join(thread, &res) == thrd_success);
    assert(res == 123);
    assert(get_counter() == 43);
    assert(external_counter == 42);
    return EXIT_SUCCESS;
}
#else
long external_counter = 0;

int main(int argc, const char **argv) {
    (void) argc;
    (void) argv;
    return EXIT_SUCCESS;
}
#endif
