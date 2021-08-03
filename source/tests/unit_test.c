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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "kefir/test/unit_test.h"

static void *kft_malloc(struct kefir_mem *mem, kefir_size_t sz) {
    UNUSED(mem);
    return malloc(sz);
}

static void *kft_calloc(struct kefir_mem *mem, kefir_size_t num, kefir_size_t sz) {
    UNUSED(mem);
    return calloc(num, sz);
}

static void *kft_realloc(struct kefir_mem *mem, void *ptr, kefir_size_t sz) {
    UNUSED(mem);
    return realloc(ptr, sz);
}

static void kft_free(struct kefir_mem *mem, void *ptr) {
    UNUSED(mem);
    free(ptr);
}

struct kefir_mem kft_mem = {
    .malloc = kft_malloc, .calloc = kft_calloc, .realloc = kft_realloc, .free = kft_free, .data = NULL};

#ifndef KFT_NOFORK
kefir_result_t kft_run_test_case(const struct kft_test_case *testCase, void *testContext) {
    printf("%s... ", testCase->description);
    fflush(stdout);
    kefir_result_t status = KEFIR_UNKNOWN_ERROR;
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("Failed (Internal error: fork() failed)\n");
        exit(-1);
    } else if (child_pid == 0) {
        status = testCase->run(testCase, testContext);
        exit((int) status);
    } else {
        int child_status;
        pid_t waited_pid = waitpid(child_pid, &child_status, 0);
        if (waited_pid < 0) {
            perror("Failed (Internal error: waitpid() failed)\n");
            exit(-1);
        }
        if (WIFEXITED(child_status)) {
            if (WEXITSTATUS(child_status) == 0) {
                printf("Ok\n");
                status = KEFIR_OK;
            } else {
                status = (kefir_result_t) WEXITSTATUS(child_status);
                printf("Failed (%d)\n", status);
            }
        } else if (WIFSIGNALED(child_status)) {
            int sig = WTERMSIG(child_status);
            printf("Terminated (signal %d)\n", sig);
            status = KEFIR_UNKNOWN_ERROR;
        } else {
            printf("Unknown\n");
            status = KEFIR_UNKNOWN_ERROR;
        }
    }
    return status;
}
#else
kefir_result_t kft_run_test_case(const struct kft_test_case *testCase, void *testContext) {
    printf("%s... ", testCase->description);
    kefir_result_t status = testCase->run(testCase, testContext);
    if (status == KEFIR_OK) {
        printf("Ok\n");
    } else {
        printf("Failed (%d)\n", status);
    }
    return status;
}
#endif

kefir_size_t kft_run_test_suite(const struct kft_test_case **testSuite, kefir_size_t testSuiteLength,
                                void *testContext) {
    printf("Running suite of %zu test(s)\n", testSuiteLength);
    kefir_size_t success = 0;
    for (kefir_size_t i = 0; i < testSuiteLength; i++) {
        if (kft_run_test_case(testSuite[i], testContext) == KEFIR_OK) {
            success++;
        }
    }
    printf("Total: %zu, Success: %zu, Failed: %zu\n", testSuiteLength, success, testSuiteLength - success);
    return testSuiteLength - success;
}
