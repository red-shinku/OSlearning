#!/bin/bash
# lock of the buffer
# lock_file="../resrc/buflock"
# exec 4>"$lock_file";

val="$1"

function produce () {
    # echo "[producer] i will produce $val";
    
    bin/_sem_wait_.sh "resrc/space.sem";
    bin/_sem_wait_.sh "resrc/free.sem";

    echo "$val" >> resrc/buffer.txt;
    echo "[producer] put $val";

    bin/_sem_post_.sh "resrc/free.sem";
    bin/_sem_post_.sh "resrc/data.sem";
}

while true; do
    produce
    sleep 0.1s
done
