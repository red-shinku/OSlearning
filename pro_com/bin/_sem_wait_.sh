#!/bin/bash
# lock of the sem_wait
lock_file="../resrc/lock";
exec 4>"$lock_file";

# function _sem_wait_ () {
#     sem_val_file=$1;
#     sem_val=$(cat "$sem_val_file");

#     flock 4;
#     if [ "$sem_val" -gt 0 ]; then
#         echo $((sem_val-1)) > "$sem_val_file";
#         flock -u 4;
#     else
        # check the sem_val_file
#     fi
# }

function _sem_wait_ () {
    sem_val_file=$1;

    while true; do
        flock 4;
        
        sem_val=$(cat "$sem_val_file");
        if [ "$sem_val" -gt 0 ]; then
            echo $((sem_val-1)) > "$sem_val_file";
            flock -u 4;
            break;
        fi
        
        flock -u 4
    done
}

_sem_wait_ "$1"