#!/usr/bin/env bash

set -e

STAGE1="$1"
STAGE2="$2"

find "$STAGE1" -name "*.s" -print0 | while read -d $'\0' stage1file
do
    stage2file="$STAGE2/$(realpath --relative-to=$STAGE1 $stage1file)"
    diff -u "$stage1file" "$stage2file"
done