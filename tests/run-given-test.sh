#!/bin/bash

if [ -z "$1" ]
    then
        echo "The path to the binary to test must be supplied."
        exit 1
fi

if [ -z "$2" ]
    then
        echo "The path to the test must be supplied."
        exit 1
fi

cat $2 | $1 ../minecraft-daemon.conf; 
