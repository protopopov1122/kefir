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

#ifndef KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_H_
#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_H_

#include "kefir/ir/type.h"
#include "kefir/ast/type.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast-translator/function_declaration.h"

typedef enum kefir_ast_translator_resolved_type_class {
    KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
    KEFIR_AST_TRANSLATOR_RESOLVED_FUNCTION_TYPE
} kefir_ast_translator_resolved_type_class_t;

typedef struct kefir_ast_translator_resolved_type {
    kefir_ast_translator_resolved_type_class_t klass;
    kefir_bool_t owner;
    const struct kefir_ast_type *type;
    union {
        struct {
            kefir_id_t ir_type_id;
            struct kefir_ir_type *ir_type;
            struct kefir_ast_type_layout *layout;
        } object;

        struct {
            struct kefir_ast_translator_function_declaration *declaration;
        } function;
    };
} kefir_ast_translator_resolved_type_t;

typedef struct kefir_ast_translator_type_resolver {
    kefir_result_t (*register_object)(struct kefir_mem *, const struct kefir_ast_translator_type_resolver *, kefir_id_t,
                                      struct kefir_ir_type *, struct kefir_ast_type_layout *);

    kefir_result_t (*free)(struct kefir_mem *, struct kefir_ast_translator_type_resolver *);

    void *payload;
} kefir_ast_translator_type_resolver_t;

#define KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_FREE(_mem, _resolver) ((_resolver)->free((_mem), (_resolver)))

#endif
