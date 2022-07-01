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

TESTDIR="$(dirname $0)"
OUTPUT_FILE="$(mktemp)"
DIFF_FILE="$(mktemp)"
VALGRIND_FILE="$(mktemp)"
DIFF="diff -u"
VALGRIND="valgrind $VALGRIND_OPTIONS --log-file=$VALGRIND_FILE"

function cleanup {
    rm -rf "$OUTPUT_FILE"
    rm -rf "$DIFF_FILE"
    rm -rf "$VALGRIND_FILE"
}

function run_valgrind_test {
    INT_TEST="$1"
    KEFIR_DISABLE_LONG_DOUBLE=1 $VALGRIND "$INT_TEST" > "$OUTPUT_FILE" 2>&1
    if [[ "x$?" != "x0" ]]; then
        echo "Terminated"
        cat "$OUTPUT_FILE"
        cat "$VALGRIND_FILE"
        exit 127
    fi
}

function run_test {
    INT_TEST="$1"
    KEFIR_DISABLE_LONG_DOUBLE=1 "$INT_TEST" > "$OUTPUT_FILE" 2>&1
    if [[ "x$?" != "x0" ]]; then
        echo "Terminated"
        cat "$OUTPUT_FILE"
        exit 127
    fi
}

trap cleanup EXIT HUP INT QUIT PIPE TERM

for INT_TEST in "$@"
do
    printf "Running $INT_TEST..."
    if [[ "x$MEMCHECK" != "x" ]]; then
        run_valgrind_test "$INT_TEST"
    else
        run_test "$INT_TEST"
    fi
    $DIFF "$OUTPUT_FILE" "$TESTDIR/$(basename $(dirname $INT_TEST))/$(basename $INT_TEST).result" > "$DIFF_FILE" 2>&1
    if [[ "x$?" != "x0" ]]; then
        echo "Failed"
        cat "$DIFF_FILE"
        exit 128
    else
        echo "Ok"
    fi 
done