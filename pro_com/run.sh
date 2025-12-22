#!/bin/bash
# please set the buffer size
bufsize=10

# sem init value
free=1
space=$bufsize
data=0

echo $free > ./resrc/free.sem
echo $space > ./resrc/space.sem
echo $data > ./resrc/data.sem

flag=0
export flag
./bin/producer.sh a &
./bin/comsumer.sh &

read flag
wait
