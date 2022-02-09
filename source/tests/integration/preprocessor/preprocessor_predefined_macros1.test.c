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

#include "kefir/core/platform.h"
#ifdef KEFIR_LINUX_HOST_PLATFORM
#define _XOPEN_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/preprocessor/virtual_source_file.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/preprocessor/format.h"
#include "kefir/preprocessor/ast_context.h"
#include "kefir/test/util.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    setenv("TZ", "UTC", 1);
    tzset();

    const char CONTENT[] = "__FILE__ __LINE__\n"
                           "__FILE__ __LINE__\t__LINE__\n"
                           "__FILE__ __LINE__\n"
                           "__DATE__ __TIME__\n"
                           "__STDC__ __STDC_HOSTED__ __STDC_VERSION__\n"
                           "utf16 = __STDC_UTF_16__\n"
                           "utf32 = __STDC_UTF_32__\n"
                           "analyzable = __STDC_ANALYZABLE__\n"
                           "iec559 = __STDC_IEC_559__\n"
                           "iec559_complex = __STDC_IEC_559_COMPLEX__\n"
                           "lib_ext1 = __STDC_LIB_EXT1__\n"
                           "no_atomics = __STDC_NO_ATOMICS__\n"
                           "no_complex = __STDC_NO_COMPLEX__\n"
                           "no_threads = __STDC_NO_THREADS__\n"
                           "no_vla = __STDC_NO_VLA__\n"
                           "kefircc = __KEFIRCC__\n"
                           "__ILP32__ __LLP64__ __LP64__ __ILP64__ __SILP64__";

    struct kefir_symbol_table symbols;
    struct kefir_lexer_context parser_context;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_context_default(&parser_context));
    REQUIRE_OK(kefir_token_buffer_init(&tokens));

    struct kefir_preprocessor_virtual_source_locator virtual_source;
    struct kefir_preprocessor_context context;
    struct kefir_preprocessor preprocessor;
    struct kefir_lexer_source_cursor cursor;
    struct kefir_preprocessor_ast_context ast_context;
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    REQUIRE_OK(kefir_preprocessor_ast_context_init(mem, &ast_context, &symbols, kefir_util_default_type_traits(),
                                                   &env.target_env, NULL));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_init(&virtual_source));
    REQUIRE_OK(kefir_preprocessor_context_init(mem, &context, &virtual_source.locator, &ast_context.context, NULL));

    context.environment.data_model = kefir_util_default_data_model();

    const char *DATETIME = "Sep 26 2021 10:42:59 UTC";
    const char *FMT = "%b %e %Y %H:%M:%S %Z";
    struct tm time = {0};
    strptime(DATETIME, FMT, &time);

    context.environment.timestamp = mktime(&time);
    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), "fileName"));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context, &context, NULL, NULL));
    REQUIRE_OK(kefir_preprocessor_run(mem, &preprocessor, &tokens));

    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));
    REQUIRE_OK(kefir_preprocessor_context_free(mem, &context));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_free(mem, &virtual_source));

    REQUIRE_OK(kefir_preprocessor_format(stdout, &tokens, KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_ORIGINAL));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_preprocessor_ast_context_free(mem, &ast_context));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
