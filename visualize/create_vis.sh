#!/bin/bash
mkdir -p maps
#rm maps/*
g++ -std=c++20 batch_generator.cpp
./a.out 50
rm a.out
python3 visualizer.py
last_file=$(ls maps -t | head -n1)
firefox maps/$last_file