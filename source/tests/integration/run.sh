#!/usr/bin/env bash

TESTDIR="$(dirname $0)"
OUTPUT_FILE="$(mktemp)"
DIFF_FILE="$(mktemp)"
VALGRIND_FILE="$(mktemp)"
DIFF="diff -u -B"
VALGRIND="valgrind --trace-children=yes --track-origins=yes --leak-check=full --error-exitcode=127 --log-file=$VALGRIND_FILE"

function cleanup {
    rm -rf "$OUTPUT_FILE"
    rm -rf "$DIFF_FILE"
    rm -rf "$VALGRIND_FILE"
}

function run_valgrind_test {
    INT_TEST="$1"
    $VALGRIND "$INT_TEST" > "$OUTPUT_FILE" 2>&1
    if [[ "x$?" != "x0" ]]; then
        echo "Terminated"
        cat "$OUTPUT_FILE"
        cat "$VALGRIND_FILE"
        exit 127
    fi
}

function run_test {
    INT_TEST="$1"
    "$INT_TEST" > "$OUTPUT_FILE" 2>&1
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