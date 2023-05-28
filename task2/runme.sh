#!/bin/bash
make

for i in {1..10}
do
./main fivemintest.txt &
done

sleep 5m

killall -SIGINT main
