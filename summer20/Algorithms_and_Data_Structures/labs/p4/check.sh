#!/bin/bash

for (( i=0; i<$1; i++ ))
do
    ./main < in/$i.in > mine/$i.out;
    python3 check.py $i;
done