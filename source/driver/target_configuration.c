/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

#define _POSIX_SOURCE
#include "kefir/driver/target_configuration.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/platform/filesystem.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

kefir_result_t kefir_driver_apply_target_profile_configuration(
    struct kefir_compiler_runner_configuration *compiler_config, const struct kefir_driver_target *target) {
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver target"));

    if (compiler_config != NULL && target->arch == KEFIR_DRIVER_TARGET_ARCH_X86_64) {
        if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_LINUX ||
            target->platform == KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD) {
            compiler_config->target_profile = "amd64-sysv-gas";
            compiler_config->codegen.emulated_tls = false;
        } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD) {
            compiler_config->target_profile = "amd64-sysv-gas";
            compiler_config->codegen.emulated_tls = true;
        }
    }

    return KEFIR_OK;
}

static kefir_result_t add_include_paths(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                        struct kefir_compiler_runner_configuration *compiler_config,
                                        const char *paths) {
    struct kefir_filesystem_path_list_iter iter;
    kefir_size_t length;
    char buffer[PATH_MAX + 1];
    for (const char *path = kefir_filesystem_path_list_iter_init(&iter, paths, ';', &length); path != NULL;
         path = kefir_filesystem_path_list_iter_next(&iter, &length)) {

        if (length == 0) {
            continue;
        }
        length = MIN(length, PATH_MAX);
        strncpy(buffer, path, length);
        buffer[length] = '\0';

        const char *path_copy = kefir_symbol_table_insert(mem, symbols, buffer, NULL);
        REQUIRE(path_copy != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert path into symbol table"));
        REQUIRE_OK(kefir_list_insert_after(mem, &compiler_config->include_path,
                                           kefir_list_tail(&compiler_config->include_path), (void *) path_copy));
    }
    return KEFIR_OK;
}

static kefir_result_t add_library_paths(struct kefir_mem *mem, struct kefir_driver_linker_configuration *linker_config,
                                        const char *paths) {
    struct kefir_filesystem_path_list_iter iter;
    kefir_size_t length;
    char buffer[PATH_MAX + 1];
    for (const char *path = kefir_filesystem_path_list_iter_init(&iter, paths, ';', &length); path != NULL;
         path = kefir_filesystem_path_list_iter_next(&iter, &length)) {

        if (length == 0) {
            continue;
        }
        length = MIN(length, PATH_MAX);
        strncpy(buffer, path, length);
        buffer[length] = '\0';

        REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-L"));
        REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, buffer));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_driver_apply_target_compiler_configuration(
    struct kefir_mem *mem, struct kefir_symbol_table *symbols, const struct kefir_driver_external_resources *externals,
    struct kefir_compiler_runner_configuration *compiler_config, const struct kefir_driver_target *target) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(symbols != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid symbol table"));
    REQUIRE(externals != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver externals"));
    REQUIRE(compiler_config != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver compiler runner configuration"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver target"));

    REQUIRE_OK(kefir_driver_apply_target_profile_configuration(compiler_config, target));

    if (target->arch == KEFIR_DRIVER_TARGET_ARCH_X86_64) {
        REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__x86_64__", "1"));
        REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__amd64__", "1"));
    }

    if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_LINUX) {
        REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__linux__", "1"));

        if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_GNU) {
            if (compiler_config != NULL) {
                REQUIRE(externals->gnu.include_path != NULL,
                        KEFIR_SET_ERROR(KEFIR_UI_ERROR, "GNU include path shall be passed as KEFIR_GNU_INCLUDE "
                                                        "environment variable for selected target"));

                REQUIRE_OK(add_include_paths(mem, symbols, compiler_config, externals->gnu.include_path));
            }
        } else if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_MUSL) {
            REQUIRE(externals->musl.include_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Musl library path shall be passed as KEFIR_MUSL_INCLUDE "
                                                    "environment variable for selected target"));

            REQUIRE_OK(add_include_paths(mem, symbols, compiler_config, externals->musl.include_path));
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD) {
        REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__FreeBSD__", "1"));
        if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
            REQUIRE(externals->freebsd.include_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "GNU include path shall be passed as KEFIR_FREEBSD_INCLUDE "
                                                    "environment variable for selected target"));

            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC__", "4"));
            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC_MINOR__", "20"));
            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC_STDC_INLINE__", "1"));

            REQUIRE_OK(add_include_paths(mem, symbols, compiler_config, externals->freebsd.include_path));
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD) {
        REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__OpenBSD__", "1"));
        if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
            REQUIRE(externals->openbsd.include_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "GNU include path shall be passed as KEFIR_OPENBSD_INCLUDE "
                                                    "environment variable for selected target"));

            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC__", "4"));
            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC_MINOR__", "20"));
            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC_STDC_INLINE__", "1"));

            REQUIRE_OK(add_include_paths(mem, symbols, compiler_config, externals->openbsd.include_path));
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_NETBSD) {
        REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__NetBSD__", "1"));
        if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
            REQUIRE(externals->netbsd.include_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "GNU include path shall be passed as KEFIR_NETBSD_INCLUDE "
                                                    "environment variable for selected target"));

            REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, compiler_config, "__GNUC__", "2"));

            REQUIRE_OK(add_include_paths(mem, symbols, compiler_config, externals->netbsd.include_path));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_driver_apply_target_assembler_configuration(
    struct kefir_mem *mem, struct kefir_symbol_table *symbols, const struct kefir_driver_external_resources *externals,
    struct kefir_driver_assembler_configuration *assembler_config, const struct kefir_driver_target *target) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(symbols != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid symbol table"));
    REQUIRE(externals != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver externals"));
    REQUIRE(assembler_config != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver assembler configuration"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver target"));

    return KEFIR_OK;
}

#define LINK_FILE(_path, _filename)                                                                             \
    do {                                                                                                        \
        const char *filepath = NULL;                                                                            \
        kefir_result_t res = kefir_filesystem_find_in_path_list(mem, symbols, (_path), (_filename), &filepath); \
        if (res == KEFIR_NOT_FOUND) {                                                                           \
            res = KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Unable to find %s in GNU library path", (_filename));       \
        }                                                                                                       \
        REQUIRE_OK(res);                                                                                        \
        REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, filepath));               \
    } while (0)

kefir_result_t kefir_driver_apply_target_linker_initial_configuration(
    struct kefir_mem *mem, struct kefir_symbol_table *symbols, const struct kefir_driver_external_resources *externals,
    struct kefir_driver_linker_configuration *linker_config, const struct kefir_driver_target *target) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(symbols != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid symbol table"));
    REQUIRE(externals != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver externals"));
    REQUIRE(linker_config != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver linker configuration"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver target"));

    if (linker_config->flags.static_linking) {
        REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-static"));
    }

    if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_LINUX) {
        if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_GNU) {
            REQUIRE(externals->gnu.library_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "GNU library path shall be passed as KEFIR_GNU_LIB "
                                                    "environment variable for selected target"));

            REQUIRE_OK(add_library_paths(mem, linker_config, externals->gnu.library_path));

            if (linker_config->flags.link_start_files) {
                LINK_FILE(externals->gnu.library_path, "crt1.o");
                LINK_FILE(externals->gnu.library_path, "crti.o");
                if (!linker_config->flags.static_linking) {
                    LINK_FILE(externals->gnu.library_path, "crtbegin.o");
                } else {
                    LINK_FILE(externals->gnu.library_path, "crtbeginS.o");
                }
            }

            if (externals->gnu.dynamic_linker != NULL) {
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "--dynamic-linker"));
                REQUIRE_OK(
                    kefir_driver_linker_configuration_add_argument(mem, linker_config, externals->gnu.dynamic_linker));
            }
        } else if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_MUSL) {
            REQUIRE(externals->musl.library_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Musl library path shall be passed as KEFIR_MUSL_LIB "
                                                    "environment variable for selected target"));

            REQUIRE_OK(add_library_paths(mem, linker_config, externals->musl.library_path));

            if (linker_config->flags.link_start_files) {
                LINK_FILE(externals->musl.library_path, "crt1.o");
                LINK_FILE(externals->musl.library_path, "crti.o");
            }

            if (externals->musl.dynamic_linker != NULL) {
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "--dynamic-linker"));
                REQUIRE_OK(
                    kefir_driver_linker_configuration_add_argument(mem, linker_config, externals->musl.dynamic_linker));
            }
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD &&
               target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
        REQUIRE(externals->freebsd.library_path != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "System library path shall be passed as KEFIR_FREEBSD_LIB "
                                                "environment variable for selected target"));

        REQUIRE_OK(add_library_paths(mem, linker_config, externals->freebsd.library_path));

        if (linker_config->flags.link_start_files) {
            LINK_FILE(externals->freebsd.library_path, "crt1.o");
            LINK_FILE(externals->freebsd.library_path, "crti.o");
            if (!linker_config->flags.static_linking) {
                LINK_FILE(externals->freebsd.library_path, "crtbegin.o");
            } else {
                LINK_FILE(externals->freebsd.library_path, "crtbeginS.o");
            }
        }

        if (externals->freebsd.dynamic_linker != NULL) {
            REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "--dynamic-linker"));
            REQUIRE_OK(
                kefir_driver_linker_configuration_add_argument(mem, linker_config, externals->freebsd.dynamic_linker));
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD &&
               target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
        REQUIRE(externals->openbsd.library_path != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "System library path shall be passed as KEFIR_OPENBSD_LIB "
                                                "environment variable for selected target"));

        REQUIRE_OK(add_library_paths(mem, linker_config, externals->openbsd.library_path));

        REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-no-pie"));
        if (linker_config->flags.link_start_files) {
            LINK_FILE(externals->openbsd.library_path, "crt0.o");
            if (!linker_config->flags.static_linking) {
                LINK_FILE(externals->openbsd.library_path, "crtbegin.o");
            } else {
                LINK_FILE(externals->openbsd.library_path, "crtbeginS.o");
            }
        }

        if (externals->openbsd.dynamic_linker != NULL) {
            REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "--dynamic-linker"));
            REQUIRE_OK(
                kefir_driver_linker_configuration_add_argument(mem, linker_config, externals->openbsd.dynamic_linker));
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_NETBSD &&
               target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
        REQUIRE(externals->netbsd.library_path != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "System library path shall be passed as KEFIR_NETBSD_LIB "
                                                "environment variable for selected target"));

        REQUIRE_OK(add_library_paths(mem, linker_config, externals->netbsd.library_path));

        if (linker_config->flags.link_start_files) {
            LINK_FILE(externals->netbsd.library_path, "crt0.o");
            LINK_FILE(externals->netbsd.library_path, "crti.o");
            if (!linker_config->flags.static_linking) {
                LINK_FILE(externals->netbsd.library_path, "crtbegin.o");
            } else {
                LINK_FILE(externals->netbsd.library_path, "crtbeginS.o");
            }
        }

        if (externals->netbsd.dynamic_linker != NULL) {
            REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "--dynamic-linker"));
            REQUIRE_OK(
                kefir_driver_linker_configuration_add_argument(mem, linker_config, externals->netbsd.dynamic_linker));
        }
    }

    return KEFIR_OK;
}

kefir_result_t kefir_driver_apply_target_linker_final_configuration(
    struct kefir_mem *mem, struct kefir_symbol_table *symbols, const struct kefir_driver_external_resources *externals,
    struct kefir_driver_linker_configuration *linker_config, const struct kefir_driver_target *target) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(symbols != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid symbol table"));
    REQUIRE(externals != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver externals"));
    REQUIRE(linker_config != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver linker configuration"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid driver target"));

    if (linker_config->flags.link_rtlib && target->variant != KEFIR_DRIVER_TARGET_VARIANT_NONE) {
        REQUIRE(externals->runtime_library != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Kefir runtime library path shall be passed as KEFIR_RTLIB "
                                                "environment variable for selected target"));
        REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, externals->runtime_library));
    }

    if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_LINUX) {
        if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_GNU) {
            REQUIRE(externals->gnu.library_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "GNU library path shall be passed as KEFIR_GNU_LIB "
                                                    "environment variable for selected target"));

            if (linker_config->flags.link_default_libs) {
                if (linker_config->flags.link_libc) {
                    REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lc"));
                    REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lm"));
                }
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-ldl"));
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lgcc"));
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lgcc_eh"));
            }

            if (linker_config->flags.link_start_files) {
                if (!linker_config->flags.static_linking) {
                    LINK_FILE(externals->gnu.library_path, "crtend.o");
                } else {
                    LINK_FILE(externals->gnu.library_path, "crtendS.o");
                }
                LINK_FILE(externals->gnu.library_path, "crtn.o");
            }

        } else if (target->variant == KEFIR_DRIVER_TARGET_VARIANT_MUSL) {
            REQUIRE(externals->musl.library_path != NULL,
                    KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Musl library path shall be passed as KEFIR_MUSL_LIB "
                                                    "environment variable for selected target"));

            if (linker_config->flags.link_default_libs && linker_config->flags.link_libc) {
                LINK_FILE(externals->musl.library_path, "libc.a");
            }

            if (linker_config->flags.link_start_files) {
                LINK_FILE(externals->musl.library_path, "crtn.o");
            }
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD &&
               target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
        REQUIRE(externals->freebsd.library_path != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "System library path shall be passed as KEFIR_FREEBSD_LIB "
                                                "environment variable for selected target"));

        if (linker_config->flags.link_default_libs) {
            if (linker_config->flags.link_libc) {
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lc"));
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lm"));
            }
        }

        if (linker_config->flags.link_start_files) {
            if (!linker_config->flags.static_linking) {
                LINK_FILE(externals->freebsd.library_path, "crtend.o");
            } else {
                LINK_FILE(externals->freebsd.library_path, "crtendS.o");
            }
            LINK_FILE(externals->freebsd.library_path, "crtn.o");
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD &&
               target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
        REQUIRE(externals->openbsd.library_path != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "System library path shall be passed as KEFIR_OPENBSD_LIB "
                                                "environment variable for selected target"));

        if (linker_config->flags.link_default_libs) {
            if (linker_config->flags.link_libc) {
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lcompiler_rt"));
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lc"));
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lm"));
            }
        }

        if (linker_config->flags.link_start_files) {
            if (!linker_config->flags.static_linking) {
                LINK_FILE(externals->openbsd.library_path, "crtend.o");
            } else {
                LINK_FILE(externals->openbsd.library_path, "crtendS.o");
            }
        }
    } else if (target->platform == KEFIR_DRIVER_TARGET_PLATFORM_NETBSD &&
               target->variant == KEFIR_DRIVER_TARGET_VARIANT_SYSTEM) {
        REQUIRE(externals->netbsd.library_path != NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "System library path shall be passed as KEFIR_NETBSD_LIB "
                                                "environment variable for selected target"));

        if (linker_config->flags.link_default_libs) {
            if (linker_config->flags.link_libc) {
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lc"));
                REQUIRE_OK(kefir_driver_linker_configuration_add_argument(mem, linker_config, "-lm"));
            }
        }

        if (linker_config->flags.link_start_files) {
            LINK_FILE(externals->netbsd.library_path, "crtn.o");
            if (!linker_config->flags.static_linking) {
                LINK_FILE(externals->netbsd.library_path, "crtend.o");
            } else {
                LINK_FILE(externals->netbsd.library_path, "crtendS.o");
            }
        }
    }
    return KEFIR_OK;
}

#undef LINK_FILE
