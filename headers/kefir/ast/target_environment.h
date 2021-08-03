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

#ifndef KEFIR_AST_TARGET_ENVIRONMENT_H_
#define KEFIR_AST_TARGET_ENVIRONMENT_H_

#include "kefir/ast/type.h"
#include "kefir/ast/designator.h"
#include "kefir/ast/object.h"

typedef void *kefir_ast_target_environment_opaque_type_t;

typedef struct kefir_ast_target_environment {
    kefir_result_t (*get_type)(struct kefir_mem *, const struct kefir_ast_target_environment *,
                               const struct kefir_ast_type *, kefir_ast_target_environment_opaque_type_t *);
    kefir_result_t (*free_type)(struct kefir_mem *, const struct kefir_ast_target_environment *,
                                kefir_ast_target_environment_opaque_type_t);
    kefir_result_t (*object_info)(struct kefir_mem *, const struct kefir_ast_target_environment *,
                                  kefir_ast_target_environment_opaque_type_t, const struct kefir_ast_designator *,
                                  struct kefir_ast_target_environment_object_info *);
    kefir_result_t (*object_offset)(struct kefir_mem *, const struct kefir_ast_target_environment *,
                                    kefir_ast_target_environment_opaque_type_t, kefir_int64_t, kefir_int64_t *);
    void *payload;
} kefir_ast_target_environment_t;

#define KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, env, type, info) ((env)->get_type((mem), (env), (type), (info)))
#define KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, env, info) ((env)->free_type((mem), (env), (info)))
#define KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, env, type, designator, info) \
    ((env)->object_info((mem), (env), (type), (designator), (info)))
#define KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_OFFSET(mem, env, type, index, offset_ptr) \
    ((env)->object_offset((mem), (env), (type), (index), (offset_ptr)))

#endif
