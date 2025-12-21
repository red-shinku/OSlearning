#!/bin/bash
echo compiling...
gcc main.c src/LRU.c src/stack.c src/tools.c -o LRUrun.exe
echo "finish! ---- ./LRUrun.exe" 