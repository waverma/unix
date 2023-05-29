#!/bin/bash
make
bash createA.sh > fileA
./main fileA fileB
gzip -k fileA fileB
gzip -cd fileB.gz | ./main fileC
./main fileA fileD -b 100
stat file? file?.gz > result.txt
