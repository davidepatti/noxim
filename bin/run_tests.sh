#!/bin/bash

mkdir -p out_tt

for f in tt/*
    do ./noxim -seed 0 -winoc -traffic table $f > out_$f
done
