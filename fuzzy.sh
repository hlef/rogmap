#!/bin/bash

i="0"

while [ $i -lt 1000 ]
do
    ./rogmap
    sleep 0.1
    i=$[$i+1]
done
