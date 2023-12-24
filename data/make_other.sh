#!/bin/bash

echo before make_other:
find datasets -type f -name '*.csv' -exec wc -l {} +
find datasets -type f -not -name 'moscow_claims_2023-09-01.csv' -delete
g++ -std=c++20 make_other.cpp
./a.out
rm  a.out
find datasets -type f -name '*.csv' -exec wc -l {} +