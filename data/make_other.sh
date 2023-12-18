#!/bin/bash

g++ -std=c++20 make_other.cpp
./a.out
rm  a.out
find datasets -type f -name '*.csv' -exec wc -l {} +