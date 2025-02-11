#!/bin/bash

for i in $(seq 2 16); do
  g++ main.cpp -o main -O2 -D COUNTER_PER_BUCKET=$i
  ./main
done