#!/bin/bash

OUT_DIR=out_tt
IN_DIR=tt
FILE_PREFIX=tt_
FILE_POSTFIX=.txt

mkdir -p $OUT_DIR

if [ $# -eq 0 ]
then
    for F in $IN_DIR/*
    do
        echo "Traffic Table" $F "is:"
        cat "$F"

        ./noxim -seed 0 -winoc -traffic table "$F" > "$OUT_DIR/$(basename $F)"
        echo
    done
else
    for VAR in "$@"
    do
        FILENAME=$FILE_PREFIX$VAR$FILE_POSTFIX
        echo "Traffic Table" $FILENAME "is:"
        cat "$IN_DIR/$FILENAME"
        ./noxim -seed 0 -winoc -traffic table "$IN_DIR/$FILENAME" > "$OUT_DIR/$FILENAME"
        echo
    done
fi
