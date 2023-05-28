#!/bin/bash
gcc myprogram.c -o myprogram
bash createA.sh > fileA
./myprogram fileA fileB
gzip -k fileA fileB
gzip -cd fileB.gz | ./myprogram fileC
./myprogram fileA fileD -b 100
stat file? file?.gz > result.txt
