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

./bin/producer.sh a &
./bin/producer.sh b &
./bin/producer.sh c &
./bin/producer.sh d &
./bin/comsumer.sh &
./bin/comsumer.sh &
./bin/comsumer.sh &
./bin/comsumer.sh &

wait
