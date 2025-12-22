#!/bin/bash
# please set the buffer size
bufsize=10

# sem init value
free=1
space=$bufsize
data=0

cat $free > ./resrc/free.sem
cat $space > ./resrc/space.sem
cat $data > ./resrc/data.sem

./bin/producer.sh a &
./bin/comsumer.sh &

wait
