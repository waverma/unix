#!/bin/bash
rm result.txt
make

echo "remove lock file test"
./main removelockfiletest.txt & rm removelockfiletest.txt.lck

sleep 5

echo "ten proccess test"

for i in {1..10}
do
./main fivemintest.txt -l &
echo "start proccess" $i
done

sleep 5m

echo "stop all"
killall -SIGINT main
