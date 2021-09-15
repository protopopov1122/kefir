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

#ifndef KEFIR_LEXER_FORMAT_H_
#define KEFIR_LEXER_FORMAT_H_

#include "kefir/lexer/lexem.h"
#include "kefir/lexer/lexer.h"
#include "kefir/util/json.h"

kefir_result_t kefir_token_format(struct kefir_json_output *, const struct kefir_token *, kefir_bool_t);
kefir_result_t kefir_token_buffer_format(struct kefir_json_output *, const struct kefir_token_buffer *, kefir_bool_t);

#endif
