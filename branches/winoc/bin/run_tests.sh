#!/bin/bash

IN_FOLDER=TESTS
SCENARIO_PREFIX=scenario
TT_FOLDER=traffic_tables
OUT_FOLDER=out
SVER=v`svnversion -n`

mkdir -p $OUT_DIR

if [ $# -eq 0 ]
then
    NUM=*
else
    NUM=$1
fi

for SCENARIO in $IN_FOLDER/$SCENARIO_PREFIX$NUM
do
    echo
    echo "Current Scenario is : " $SCENARIO
    echo

    for TT in $SCENARIO/$TT_FOLDER/*
    do
        echo "===================="
        echo

        mkdir -p $SCENARIO/$OUT_FOLDER/$SVER/
        ./noxim -config $SCENARIO/config.yaml -seed 0 -winoc -traffic table "$TT" > "$SCENARIO/$OUT_FOLDER/$SVER/$(basename $TT)" 2>&1
        exitcode=$?

        echo "Traffic Table" $TT "is:"
        cat "$TT"
        echo

        if [ $exitcode -eq 0 ]
        then
            tail -n 9 "$SCENARIO/$OUT_FOLDER/$SVER/$(basename $TT)"
            echo
            echo
        else
            echo
            echo
            echo
        fi
    done
done

