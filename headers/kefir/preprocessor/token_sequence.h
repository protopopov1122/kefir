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

#ifndef KEFIR_PREPROCESSOR_TOKEN_SEQUENCE_H_
#define KEFIR_PREPROCESSOR_TOKEN_SEQUENCE_H_

#include "kefir/lexer/buffer.h"
#include "kefir/core/list.h"

typedef struct kefir_preprocessor_token_sequence {
    struct kefir_list buffer_stack;
} kefir_preprocessor_token_sequence_t;

kefir_result_t kefir_preprocessor_token_sequence_init(struct kefir_preprocessor_token_sequence *);
kefir_result_t kefir_preprocessor_token_sequence_free(struct kefir_mem *, struct kefir_preprocessor_token_sequence *);
kefir_result_t kefir_preprocessor_token_sequence_push_front(struct kefir_mem *,
                                                            struct kefir_preprocessor_token_sequence *,
                                                            struct kefir_token_buffer *);
kefir_result_t kefir_preprocessor_token_sequence_next(struct kefir_mem *, struct kefir_preprocessor_token_sequence *,
                                                      struct kefir_token *);
kefir_result_t kefir_preprocessor_token_sequence_shift(struct kefir_mem *, struct kefir_preprocessor_token_sequence *,
                                                       struct kefir_token_buffer *);
kefir_result_t kefir_preprocessor_token_sequence_current(struct kefir_mem *, struct kefir_preprocessor_token_sequence *,
                                                         const struct kefir_token **);

#endif
