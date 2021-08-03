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

#ifndef CODEGEN_H_
#define CODEGEN_H_

#define GENCODE(fn)                                                       \
    do {                                                                  \
        struct kefir_codegen_amd64 codegen;                               \
        struct kefir_ir_target_platform ir_target;                        \
        REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&ir_target)); \
        REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));      \
        struct kefir_ir_module module;                                    \
        REQUIRE_OK(kefir_ir_module_alloc(mem, &module));                  \
        REQUIRE_OK((fn) (mem, &module, &ir_target));                      \
        KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);            \
        REQUIRE_OK(kefir_ir_module_free(mem, &module));                   \
        KEFIR_CODEGEN_CLOSE(&codegen.iface);                              \
    } while (0)

struct function {
    const struct kefir_ast_type *type;
    struct kefir_ast_local_context local_context;
    struct kefir_list args;
    struct kefir_ast_node_base *body;
};

#endif
