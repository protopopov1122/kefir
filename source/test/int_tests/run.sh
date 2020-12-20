#!/usr/bin/env sh

TESTDIR="$(dirname $0)"
OUTPUT_FILE="$(mktemp)"
DIFF_FILE="$(mktemp)"
VALGRIND_FILE="$(mktemp)"
DIFF="diff -u -B"
VALGRIND="valgrind --trace-children=yes --leak-check=full --error-exitcode=127 --log-file=$VALGRIND_FILE"

function cleanup {
    rm -rf "$OUTPUT_FILE"
    rm -rf "$DIFF_FILE"
    rm -rf "$VALGRIND_FILE"
}

trap cleanup EXIT HUP INT QUIT PIPE TERM

for int_test in "$@"
do
    printf "Running $int_test..."
    $VALGRIND $int_test > "$OUTPUT_FILE" 2>&1
    if [[ "x$?" != "x0" ]]; then
        echo "Terminated"
        cat "$OUTPUT_FILE"
        cat "$VALGRIND_FILE"
        exit -1
    fi
    $DIFF "$OUTPUT_FILE" "$TESTDIR/$(basename $int_test).result" > "$DIFF_FILE" 2>&1
    if [[ "x$?" != "x0" ]]; then
        echo "Failed"
        cat "$DIFF_FILE"
        exit -2
    else
        echo "Ok"
    fi 
done