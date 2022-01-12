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

#ifndef KEFIR_TEST_UTIL_H_
#define KEFIR_TEST_UTIL_H_

#include "kefir/ast-translator/environment.h"

struct kefir_ir_target_platform *kft_util_get_ir_target_platform();
struct kefir_ast_translator_environment *kft_util_get_translator_environment();
const struct kefir_ast_type_traits *kefir_util_default_type_traits();

#endif
