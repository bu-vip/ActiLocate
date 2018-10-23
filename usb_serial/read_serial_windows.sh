#!/bin/bash
make
trap 'kill %1; kill %2; kill %3; kill %4; kill %5; kill %6; kill %7; kill %8; kill %9; kill %10; kill %11' EXIT
./read_sensor.sh /dev/ttyS3 ACM0.txt &
./read_sensor.sh /dev/ttyS4 ACM1.txt & 
./read_sensor.sh /dev/ttyS5 ACM2.txt &
./read_sensor.sh /dev/ttyS6 ACM3.txt &
./read_sensor.sh /dev/ttyS7 ACM4.txt &
./read_sensor.sh /dev/ttyS8 ACM5.txt &
./read_sensor.sh /dev/ttyS9 ACM6.txt &
./read_sensor.sh /dev/ttyS10 ACM7.txt &
./read_sensor.sh /dev/ttyS11 ACM8.txt &
./read_sensor.sh /dev/ttyS12 ACM9.txt &
./read_sensor.sh /dev/ttyS13 USB0.txt &
./read_sensor.sh /dev/ttyS14 USB1.txt 
#./read_sensor.sh /dev/ttyACM10 ACM10.txt &
#./read_sensor.sh /dev/ttyACM11 ACM11.txt &
