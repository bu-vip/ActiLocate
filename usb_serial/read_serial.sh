
#!/bin/bash
make
trap 'kill %1; kill %2; kill %3; kill %4; kill %5; kill %6; kill %7; kill %8; kill %9; kill %10; kill %11' SIGINT
./read_sensor.sh /dev/ttyACM0 ACM0.txt &
./read_sensor.sh /dev/ttyACM1 ACM1.txt & 
./read_sensor.sh /dev/ttyACM2 ACM2.txt &
./read_sensor.sh /dev/ttyACM3 ACM3.txt &
./read_sensor.sh /dev/ttyACM4 ACM4.txt &
./read_sensor.sh /dev/ttyACM5 ACM5.txt &
./read_sensor.sh /dev/ttyACM6 ACM6.txt &
./read_sensor.sh /dev/ttyACM7 ACM7.txt &
./read_sensor.sh /dev/ttyACM8 ACM8.txt &
./read_sensor.sh /dev/ttyACM9 ACM9.txt &
./read_sensor.sh /dev/ttyUSB0 USB0.txt &
./read_sensor.sh /dev/ttyUSB1 USB1.txt 
#./read_sensor.sh /dev/ttyACM10 ACM10.txt &
#./read_sensor.sh /dev/ttyACM11 ACM11.txt &
