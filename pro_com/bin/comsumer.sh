#!/bin/bash
# lock of the buffer
# lock_file="../resrc/buflock"
# exec 4>"$lock_file";

function comsume(){
    echo "[comsumer] i will eat";

    bin/_sem_wait_.sh "resrc/data.sem";
    bin/_sem_wait_.sh "resrc/free.sem";

    result=$(sed -n "1p" resrc/buffer.txt);
    sed -i '1d' resrc/buffer.txt;
    echo "[comsumer] eat $result";

    bin/_sem_post_.sh "resrc/free.sem";
    bin/_sem_post_.sh "resrc/space.sem";

}

while [ $flag -eq 0 ]; do
    comsume
    sleep 1.5s
done

