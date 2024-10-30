#!/bin/bash
today=$(date +"%s")
randomNumber=($today$RANDOM)
g++  -O3 beam.cpp -o beam
for (( c=1; c<6; c++ ))
do  
 ./beam $randomNumber
done

