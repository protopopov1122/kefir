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

#ifndef KEFIR_PREPROCESSOR_DIRECTIVES_H_
#define KEFIR_PREPROCESSOR_DIRECTIVES_H_

#include "kefir/preprocessor/preprocessor.h"

typedef enum kefir_preprocessor_directive_type {
    KEFIR_PREPROCESSOR_DIRECTIVE_IF,
    KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF,
    KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF,
    KEFIR_PREPROCESSOR_DIRECTIVE_ELIF,
    KEFIR_PREPROCESSOR_DIRECTIVE_ELSE,
    KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF,
    KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE,
    KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_OBJECT,
    KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_FUNCTION,
    KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF,
    KEFIR_PREPROCESSOR_DIRECTIVE_LINE,
    KEFIR_PREPROCESSOR_DIRECTIVE_ERROR,
    KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA,
    KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY,
    KEFIR_PREPROCESSOR_DIRECTIVE_NON,
    KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE,
    KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL
} kefir_preprocessor_directive_type_t;

typedef struct kefir_preprocessor_if_directive {
    struct kefir_token_buffer condition;
} kefir_preprocessor_if_directive_t;

typedef struct kefir_preprocessor_ifdef_directive {
    const char *identifier;
} kefir_preprocessor_ifdef_directive_t;

typedef struct kefir_preprocessor_define_object_directive {
    const char *identifier;
    struct kefir_token_buffer replacement;
} kefir_preprocessor_define_object_directive_t;

typedef struct kefir_preprocessor_define_function_directive {
    const char *identifier;
    struct kefir_list parameters;
    kefir_bool_t vararg;
    struct kefir_token_buffer replacement;
} kefir_preprocessor_define_function_directive_t;

typedef struct kefir_preprocessor_undef_directive {
    const char *identifier;
} kefir_preprocessor_undef_directive_t;

typedef struct kefir_preprocessor_pp_tokens_directive {
    struct kefir_token_buffer pp_tokens;
} kefir_preprocessor_pp_tokens_directive_t;

typedef struct kefir_preprocessor_directive {
    kefir_preprocessor_directive_type_t type;
    union {
        struct kefir_preprocessor_if_directive if_directive;
        struct kefir_preprocessor_ifdef_directive ifdef_directive;
        struct kefir_preprocessor_define_object_directive define_obj_directive;
        struct kefir_preprocessor_define_function_directive define_fun_directive;
        struct kefir_preprocessor_undef_directive undef_directive;
        struct kefir_preprocessor_pp_tokens_directive pp_tokens;
    };
} kefir_preprocessor_directive_t;

kefir_result_t kefir_preprocessor_next_directive(struct kefir_mem *, struct kefir_preprocessor *,
                                                 struct kefir_preprocessor_directive *);
kefir_result_t kefir_preprocessor_directive_free(struct kefir_mem *, struct kefir_preprocessor_directive *);

#endif
