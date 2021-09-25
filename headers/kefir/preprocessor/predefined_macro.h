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

#ifndef KEFIR_PREPROCESSOR_PREDEFINED_MACRO_H_
#define KEFIR_PREPROCESSOR_PREDEFINED_MACRO_H_

#include "kefir/preprocessor/macro.h"

typedef struct kefir_preprocessor kefir_preprocessor_t;

typedef struct kefir_preprocessor_predefined_macro_scope {
    struct kefir_preprocessor_macro_scope scope;
    struct kefir_preprocessor *preprocessor;

    struct {
        struct kefir_preprocessor_macro file;
        struct kefir_preprocessor_macro line;
    } macros;
} kefir_preprocessor_predefined_macro_scope_t;

kefir_result_t kefir_preprocessor_predefined_macro_scope_init(struct kefir_preprocessor_predefined_macro_scope *,
                                                              struct kefir_preprocessor *);

#endif
