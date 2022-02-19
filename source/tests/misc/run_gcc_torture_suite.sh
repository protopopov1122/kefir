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

if [[ "x$KEFIRCC" == "x" ]]; then
    echo "Define KEFIRCC environment variable"
    exit 1
fi

if [[ "x$LIBC" == "x" ]]; then
    echo "Define LIBC environment variable"
    exit 1
fi

if [[ "x$TORTURE" == "x" ]]; then
    echo "Define TORTURE environment variable"
    exit 1
fi

CC="$KEFIRCC --feature-missing-function-return-type --feature-designated-init-colons --feature-labels-as-values --feature-implicit-function-decl --feature-empty-structs --feature-ext-pointer-arithmetics --feature-missing-braces-subobj --include $(dirname $0)/torture.h"
SKIP_LIST="$(dirname $0)/torture.skip"
TIMEOUT=10
SKIPPED_TESTS=0
FAILED_TESTS=0
TOTAL_TESTS=0

KEFIRRT=runtime.o
$KEFIRCC --dump-runtime-code | as -o $KEFIRRT

ulimit -s 64000 # Stack size=64M

function is_test_skipped {
    grep "skip $1/$2" "$SKIP_LIST" >/dev/null
}

function run_test {(
    set -e
    timeout $TIMEOUT $CC -o test.s "$1"
    as -o test.o test.s
    if [[ "x$2" == "xexecute" ]]; then
        ld -o test.bin test.o $KEFIRRT $LIBC
        ./test.bin
    fi
    rm -rf test.s test.o test.bin
)}

function run_tests {
    for test_file in "$TORTURE/$1"/*.c ; do
      if is_test_skipped "$1" "$(basename $test_file)"; then
        result="Skip"
        SKIPPED_TESTS=$(( SKIPPED_TESTS + 1 ))
      else
        run_test "$test_file" "$1"
        if [ "$?" == "0" ] ; then
          result="Success"
        else
          result="Failure"
          FAILED_TESTS=$(( FAILED_TESTS + 1 ))
        fi
      fi
      TOTAL_TESTS=$(( TOTAL_TESTS + 1 ))
      echo "($FAILED_TESTS:$TOTAL_TESTS) $result $test_file"
    done
}

run_tests compile
run_tests execute
echo "Failed tests: $FAILED_TESTS; Skipped tests: $SKIPPED_TESTS; Total tests: $TOTAL_TESTS"

