#!/bin/bash
for filename in `find src | egrep '\.cpp'`; 
do 
    gcov -n -o build/src/CMakeFiles/Minecraft-Daemon.dir "${filename}.cpp" > /dev/null; 
done