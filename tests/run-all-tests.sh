#!/bin/bash

if [ -z "$1" ]
    then
        echo "The path to the binary to test must be supplied."
        exit 1
fi

for filename in `find . -type f -name '*.test'`; 
do 
    cat $filename | $1 ../minecraft-daemon.conf; 
done
