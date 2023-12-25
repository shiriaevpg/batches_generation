#!/bin/bash
mkdir -p images
g++ -std=c++20 statistics.cpp
./a.out 5000
rm a.out
python3 graphic.py