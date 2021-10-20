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

#include "kefir/cli/input.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/os_error.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

kefir_result_t kefir_cli_input_open(struct kefir_mem *mem, struct kefir_cli_input *input, const char *filepath) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(input != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid CLI input"));

    input->filepath = filepath;
    int fd;
    if (filepath != NULL) {
        fd = open(filepath, O_RDONLY);
        REQUIRE(fd >= 0, KEFIR_SET_OS_ERROR("Failed to open file"));

        struct stat statbuf;
        REQUIRE(fstat(fd, &statbuf) >= 0, KEFIR_SET_OS_ERROR("Failed to fstat file"));

        input->content = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
        REQUIRE(input->content != MAP_FAILED, KEFIR_SET_OS_ERROR("Failed to mmap file"));
        input->length = statbuf.st_size;
    } else {
        fd = STDIN_FILENO;
        input->content = NULL;
        input->length = 0;

        kefir_size_t buffer_capacity = 0;
        char chunk[256];
        ssize_t sz;
        while ((sz = read(fd, chunk, sizeof chunk)) > 0) {
            if (input->length + sz >= buffer_capacity) {
                buffer_capacity = input->length + sz + 1024;
                input->content = KEFIR_REALLOC(mem, input->content, buffer_capacity);
                REQUIRE(input->content != NULL,
                        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to reallocate input buffer"));
            }
            memcpy(((char *) input->content) + input->length, chunk, sz);
            input->length += sz;
        }
    }
    close(fd);
    return KEFIR_OK;
}

kefir_result_t kefir_cli_input_close(struct kefir_mem *mem, struct kefir_cli_input *input) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(input != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid CLI input"));

    if (input->filepath != NULL) {
        int err = munmap(input->content, input->length);
        REQUIRE(err == 0, KEFIR_SET_OS_ERROR("Failed to unmap file"));
    } else {
        KEFIR_FREE(mem, input->content);
    }
    input->filepath = NULL;
    input->content = NULL;
    input->length = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_cli_input_get(const struct kefir_cli_input *input, const char **content_ptr,
                                   kefir_size_t *length_ptr) {
    REQUIRE(input != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid CLI input"));

    ASSIGN_PTR(content_ptr, input->content);
    ASSIGN_PTR(length_ptr, input->length);
    return KEFIR_OK;
}
