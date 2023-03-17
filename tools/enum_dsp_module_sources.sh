#!/bin/bash
sources=""

for path in `find src/modules/ -not -path '*/.*' -type f -name "*_modules_*.c" | grep "\.c" | grep -v '#' | grep -v '~'`
do
    sources+="$path "
done
echo "$sources"
