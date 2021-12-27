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

#ifndef KEFIR_AST_TEMPORARIES_H_
#define KEFIR_AST_TEMPORARIES_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ast/base.h"

typedef struct kefir_ast_temporary_identifier {
    kefir_bool_t nested;
    kefir_id_t identifier;
    kefir_id_t field;
} kefir_ast_temporary_identifier_t;

typedef struct kefir_ast_context_temporaries {
    kefir_bool_t init_done;
    const struct kefir_ast_type *type;
    struct kefir_ast_struct_type *temporaries;
    struct kefir_ast_temporary_identifier current;
} kefir_ast_context_temporaries_t;

kefir_bool_t kefir_ast_temporaries_init(struct kefir_mem *, struct kefir_ast_type_bundle *, kefir_bool_t,
                                        struct kefir_ast_context_temporaries *);
kefir_result_t kefir_ast_temporaries_next_block(struct kefir_ast_context_temporaries *);
kefir_result_t kefir_ast_temporaries_new_temporary(struct kefir_mem *, const struct kefir_ast_context *,
                                                   const struct kefir_ast_type *,
                                                   struct kefir_ast_temporary_identifier *);

#endif
