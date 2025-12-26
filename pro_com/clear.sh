#!/bin/bash

pkill -f "./bin/comsumer.sh"
pkill -f "./bin/producer.sh"

> ./resrc/buffer.txt 
