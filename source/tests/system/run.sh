#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0
# 
# Copyright (C) 2020-2021  Jevgenijs Protopopovs
# 
# This file is part of Kefir project.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
# # 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

DIR="$(dirname $0)"
TMPDIR="$(mktemp -d)"
ASM_FILE="$TMPDIR/gen.asm"
ASM_OBJ="$TMPDIR/gen.o"
LIB_OBJ="$TMPDIR/lib.o"
TEST_EXE="$TMPDIR/test"
VALGRIND_FILE="$TMPDIR/gen.log"
VALGRIND="valgrind --trace-children=yes --track-origins=yes --expensive-definedness-checks=yes --leak-check=full --error-exitcode=127 --log-file=$VALGRIND_FILE"
NASM="nasm -f elf64 -o $ASM_OBJ"
COMPILE="$CC -std=c11 -Wall -Wextra -pedantic $OPT $DBG -no-pie -I$DIR/../../../headers -o $TEST_EXE"

if [[ "x$SANITIZE" == "xundefined" ]]; then
    COMPILE="$COMPILE -fsanitize=undefined -fno-sanitize-recover=all"
fi

function cleanup {
    rm -rf "$TMPDIR"
}

trap cleanup EXIT HUP INT QUIT PIPE TERM

mkdir -p "$TMPDIR"
nasm -f elf64 -o "$LIB_OBJ" "$DIR/../../runtime/amd64_sysv.asm"
if [[ "x$?" != "x0" ]]; then
    exit 126
fi

for SYS_TEST in "$@"
do
    printf "Running $SYS_TEST..."
    $VALGRIND "$SYS_TEST" > "$ASM_FILE"
    if [[ "x$?" != "x0" ]]; then
        echo "Terminated"
        cat "$ASM_FILE"
        cat "$VALGRIND_FILE"
        exit 127
    fi
    $NASM "$ASM_FILE"
    if [[ "x$?" != "x0" ]]; then
        exit 128
    fi
    TEST_FILE="$DIR/$(basename $(dirname $SYS_TEST))/$(basename $SYS_TEST .gen).test.c"
    $COMPILE $TEST_FILE $ASM_OBJ $LIB_OBJ
    if [[ "x$DISASM" == "xexe" ]]; then
        objdump -d "$TEST_EXE"
    fi
    $VALGRIND "$TEST_EXE"
    if [[ "x$?" != "x0" ]]; then
        echo "Failed"
        cat "$VALGRIND_FILE"
        if [[ "x$GDB" = "xyes" ]]; then
            gdb "$TEST_EXE"
        fi
        exit 129
    fi
    echo "Ok"
done