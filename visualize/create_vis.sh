#!/bin/bash
mkdir -p maps
g++ -std=c++20 single_batch_generator.cpp
./a.out
rm a.out
python3 visualizer.py
last_file=$(ls maps -t | head -n1)
firefox maps/$last_file