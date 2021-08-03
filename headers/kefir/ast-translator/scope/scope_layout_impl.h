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

#ifndef KEFIR_AST_TRANSLATOR_SCOPE_SCOPE_LAYOUT_IMPL_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_SCOPE_LAYOUT_IMPL_H_

#include "kefir/ast-translator/scope/scoped_identifier.h"

kefir_result_t kefir_ast_translator_scoped_identifer_payload_free(struct kefir_mem *,
                                                                  struct kefir_ast_scoped_identifier *, void *);

kefir_result_t kefir_ast_translator_scoped_identifier_remove(struct kefir_mem *, struct kefir_list *,
                                                             struct kefir_list_entry *, void *);

kefir_result_t kefir_ast_translator_scoped_identifier_insert(struct kefir_mem *, const char *,
                                                             const struct kefir_ast_scoped_identifier *,
                                                             struct kefir_list *);

#endif
