#!/bin/bash
python visualizer.py
last_file=$(ls maps -t | head -n1)
firefox maps/$last_file