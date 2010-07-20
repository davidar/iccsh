#!/bin/sh

for file in *.iccsh; do
    base=`basename $file .iccsh`
    echo -n "$base... "
    rm -f test.out
    ../iccsh < $file > test.out 2>/dev/null
    if diff -q test.out $base.out >/dev/null; then
        echo PASS
    else
        echo FAIL
        exit 1
    fi
done
