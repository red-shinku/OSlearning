#!/bin/bash
# lock of the buffer
# lock_file="../resrc/buflock"
# exec 4>"$lock_file";

val="$1"

function produce () {
    echo "[producer] i will produce $val";
    
    ./_sem_wait_.sh "../resrc/space.sem";
    ./_sem_wait_.sh "../resrc/free.sem";

    echo "$val" > ../resrc/buffer.txt;
    echo "[producer] put $val";

    ./_sem_post_.sh "../resrc/free.sem";
    ./_sem_post_.sh "../resrc/data.sem";
}

produce
