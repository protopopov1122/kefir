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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "kefir/core/util.h"
#include "kefir/compiler/compiler.h"
#include "kefir/core/os_error.h"
#include "kefir/core/error_format.h"

static kefir_result_t mmap_file(const char *filepath, const char **content, size_t *length) {
    int fd = open(filepath, O_RDONLY);
    REQUIRE(fd >= 0, KEFIR_SET_OS_ERROR("Failed to open file"));

    struct stat statbuf;
    REQUIRE(fstat(fd, &statbuf) >= 0, KEFIR_SET_OS_ERROR("Failed to fstat file"));

    const char *ptr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    REQUIRE(ptr != MAP_FAILED, KEFIR_SET_OS_ERROR("Failed to mmap file"));
    *content = ptr;
    *length = statbuf.st_size;
    close(fd);
    return KEFIR_OK;
}

static kefir_result_t unmap_file(const char *content, kefir_size_t length) {
    int err = munmap((void *) content, length);
    if (err != 0) {
        return KEFIR_SET_OS_ERROR("Failed to unmap file");
    }
    return KEFIR_OK;
}

static kefir_result_t compile_code(struct kefir_mem *mem, const char *code, kefir_size_t length) {
    struct kefir_compiler_profile profile;
    struct kefir_compiler_context compiler;
    struct kefir_ast_translation_unit *unit = NULL;
    struct kefir_ir_module module;

    REQUIRE_OK(kefir_compiler_profile(&profile, NULL));
    REQUIRE_OK(kefir_compiler_context_init(mem, &compiler, &profile));
    REQUIRE_OK(kefir_compiler_parse_source(mem, &compiler, code, length, &unit));
    REQUIRE_OK(kefir_compiler_analyze(mem, &compiler, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_compiler_translate(mem, &compiler, unit, &module));
    REQUIRE_OK(kefir_compiler_codegen(mem, &compiler, &module, stdout));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_compiler_context_free(mem, &compiler));
    return KEFIR_OK;
}

static kefir_result_t compiler(const char *filepath) {
    const char *file_content = NULL;
    kefir_size_t file_length = 0;
    REQUIRE_OK(mmap_file(filepath, &file_content, &file_length));
    REQUIRE_OK(compile_code(kefir_system_memalloc(), file_content, file_length));
    REQUIRE_OK(unmap_file(file_content, file_length));
    return KEFIR_OK;
}

static int report_error(kefir_result_t res) {
    if (res == KEFIR_OK) {
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Failed to compile! Error stack:\n");
        kefir_format_error(stderr, kefir_current_error());
    }
    return EXIT_FAILURE;
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        printf("Usage: %s file-name", argv[0]);
        return EXIT_FAILURE;
    }
    kefir_result_t res = compiler(argv[1]);
    return report_error(res);
}
