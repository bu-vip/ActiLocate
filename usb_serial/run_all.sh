#!/bin/bash

rm -rf raw/raw*

gcc -pthread arduino-serial-lib.c read_sensors.c -o read_sensors
sudo ./read_sensors

sleep 1
python3.6 parser.py
