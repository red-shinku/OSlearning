#!/bin/bash
#lock of the sem_post
lock_file="resrc/lock";
exec 4>"$lock_file";

function _sem_post_(){
    sem_val_file=$1;

    flock 4;
    sem_val=$(cat "$sem_val_file");
    echo $((sem_val+1)) > "$sem_val_file";
    flock -u 4;
}

_sem_post_ "$1"