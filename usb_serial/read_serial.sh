#!/bin/bash
make -s
if [ "$1" == 'linux' ]
then
trap 'kill %1; kill %2; kill %3; kill %4; kill %5; kill %6; kill %7; kill %8; kill %9; kill %10; kill %11' SIGINT
elif [ "$1" == 'windows' ]
then
trap 'kill %1; kill %2; kill %3; kill %4; kill %5; kill %6; kill %7; kill %8; kill %9; kill %10; kill %11' EXIT
else
echo "please specify filesystem, \"windows\" or \"linux\" "
exit
fi
# note: change the /dev/tty... to the corresponding usb files
./read_sensor.sh /dev/ttyACM0 raw/raw0.txt &
./read_sensor.sh /dev/ttyACM1 raw/raw1.txt & 
./read_sensor.sh /dev/ttyACM2 raw/raw2.txt &
./read_sensor.sh /dev/ttyACM3 raw/raw3.txt &
./read_sensor.sh /dev/ttyACM4 raw/raw4.txt &
./read_sensor.sh /dev/ttyACM5 raw/raw5.txt &
./read_sensor.sh /dev/ttyACM6 raw/raw6.txt &
./read_sensor.sh /dev/ttyACM7 raw/raw7.txt &
./read_sensor.sh /dev/ttyACM8 raw/raw8.txt &
./read_sensor.sh /dev/ttyACM9 raw/raw9.txt &
./read_sensor.sh /dev/ttyUSB0 raw/raw10.txt &
./read_sensor.sh /dev/ttyUSB1 raw/raw11.txt 

echo "parsing raw data..."
sleep 1
python3.6 parser.py
