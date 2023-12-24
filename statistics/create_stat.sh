#!/bin/bash
g++ -std=c++20 statistics.cpp
./a.out 30000
rm a.out
python3 graphic.py