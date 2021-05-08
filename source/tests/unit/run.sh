#!/usr/bin/env bash

VALGRIND="valgrind --trace-children=yes --leak-check=full --error-exitcode=127"

if [[ "x$MEMCHECK" == "xyes" ]]; then
    exec $VALGRIND "$1"
else
    exec "$1"
fi