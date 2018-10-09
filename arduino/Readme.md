## Overview
The sensor controller arduino sketch will be run on each arduino which acts as a sensor module. Each time a clock signal from the MSP432 is detected the
    arduino will poll the sensor and transfer data to the parsing server. **NOTE:** each operation performed by the arduino takes a certain amount of time, and
    the MSP will generate clock signals at a fixed frequency, with no feedback from the arduino on when the poll/data transfer operation has concluded. For this
    reason, the clock frequency of the MSP must be chosen such that the arduinos are able to complete all operations in less time than the MSP clock period.


## Components
* **ID:** The id of the sensor module (0 - 11). Must be set in the sensor controller sketch

* **Timestamp:** used by the parsing server to syncronize arduino data. Is an unsigned long integer representing the number of MSP clock signals detected since
 the last arduino power cycle, stored in the global variable "num_clocks". Each time a clock signal is detected this value increments. This value is stored in
 an arduino register so time to increment and package is ~10us.

* **LED state pin**: recieves the LED state from the MSP432. Configured on pin 4 by default with an internal pullup resistor. Can be on any digital GPIO pin,
 change the value of LED state to put on a different pin.

* **External interrupt pin:** Recieves the clock signal from the MSP432. Configured with an internal pullup resistor. Arduino Uno's are externally interruptable
 (can be configured to generate an interrupt on a voltage change) on pins 2 and 3. In out setup pin 2 is configured as the interrupt pin with the interrupt
 triggering on the rising edge. We have a global variable "doRead" set to false by default which the interrupt service routine enables then returns to the main
 loop. If doRead is true in the main loop, the arduino reads the sensor, packages the sensor data with ID and timestamp, sends it to the server over wifi,
 increaments the interrupt counter, then sets doRead false. Time to service the interrupt was found to be about 15us.

* **Sensor:** Communicates with the arduino on pins A4 (SDA) and A5 (SCL) at 40Khz I2C using the Wire library and Drivers from Adafruit. Connect power to 5V and
 ground to one of the GND pins on the arduino power bus. Configure with a specific analog gain an integration time by setting the SENSOR_INTEGRATION_TIME and
 SENSOR_GAIN_VALUES. **NOTE:** on the dccduino boards, connect sensor SDA to the dedicated board SDA pin instead of A4 and SCL to the deticated SCL pin instead
 of A5. Time to poll the sensor is aproximately 1ms (Time to send I2C data at 40Kz + arduino overhead) + sensor integration time.
To change the integration time and/or sensor gain:
Go to ./arduino/sensor_controller/sensor_controller.ino
Change the value in line 21 to change the integration time. Choices are 2.4ms, 24ms, 50ms, 101ms, 154ms, 700ms
Note: the integration time should not be longer than the state duration time; better to leave some margins
Change the value in line 28 to change the gain of sensors. Choices are 1X, 4X, 16X, 60X
Reprogram each of the 12 Arduinos

* **Wifi module:** The ESP8266 S1 wifi module is a small microcontroller that the arduino communicates with by sending AT commands over UART at a baud rate
 determined by the value of BAUD. Serial commnication to the wifi module is done via the arduino SoftwareSerial library. Software serial devices can be created
 on any digital IO pins. Our software serial device (which communicates with the wifi module) is set up on pins 5 and 6 by default, chance the ESP_RX and
 ESP_TX pins to communicated with the wifi module on different pins. **NOTE:** the wifi module needs to be configured separately to opperate at a specific baud
 rate. Just changing the value of BAUD without reconfiguring the baud rate at which the wifi module communicates will cause communication between the wifi
 module and the arduino to fail. See the installation section for more detail on how to configure the wifi module. You shouldn't need to reconfigure the wifi
 module unless you want to change the network name or password, or the baud rate at which the wifi module communicates with the arduino. The power pin of the
 wifi module is connected to 3.3V on the arduino power bus, ground pin is connected to one of the GND pins on the arduino power bus, wifi module chip select is
 connected to 3.3 V, wifi module TX is connected to the RX pin of the software serial device, and wifi module RX pin is connected to the TX pin of the software
 serial device. Because the wifi module is not 5V logic tollerant and voltages on the arduino pins are 5V, there is a voltage divider on the esp RX pin which
 steps the voltage from 5 V to 3.33V. In the setup function, each arduino sends a command to it's wifi module telling it to start a connection with the parsing
 server. The wifi modules were configured to communicate at 57600 baud, act as wifi stations/clients, and connect to the "penta-lab" network with the penta-lab
 password on boot in advance.

* AT Commands Used: **NOTE** the wifi module expects '\r\n' as line terminators on all AT commands. This can be accomplished by using the software serial
 println function (which automatically appends
'\r\n' to transmitted data). 
- AT+RST: Restarts the wifi module when the arduino boots. This insures the wifi module has the proper connections and ip. Called in the arduino setup function,
  so time to execute will not affect the MSP clock frequency
- AT+CIPSTART="<Protocol>","<IP>","<Port>": Starts a connection using TCP/UDP to an ip address/hostname on a specific port. We'recreating a UDP connection to
  the ip address and port of our parsing server running on the raspberry pi at 92.168.1.2:9100. If you want to change the ip address or port of the parsing
  server, you must reprogram each arduino with the new ip address and port. Called in the setup function, so time to execute does not affect the MSP clock
  frequency
- AT+CIPSEND=<length>: Sends <length> bytes of data over the connection you created with CIPSTART. This command is called in the main loop when a clock signal
  is detected. In our case length is 14 Time to execute is 780 us + 280 us(time to set the AT command over UART at 57600 BAUD plus time to get a response from
  the wifi module plus time to transmit 14 bytes at 57600 BAUD). **NOTE:** these values were determined experimentally for a specific baud rate and data length.
  If you wish to slow down the baud rate you must determine the new values.

* **Transmitted Data Structure:** Global variable "data" used to store data transmitted to the parsing server. Has a length of 14 bytes.
 * data.packet_size[0]: sensor module ID (0 - 11)
 * data.packet_size[1-4]: sensor module timestamp (# of clocks), little-endian
 * data.packet_size[5]: LED state (0/1)
 * data.packet_size[6-7]: Red color value from sensor, little-endian
 * data.packet_size[8-9]: Green color value from sensor, little-endian
 * data.packet_size[10-11]: Blue color value from sensor, little-endian
 * data.packet_size[12-13]: Clear color value from sensor, little-endian

## Installation
We will be using Adafruit's TCS library available at https://github.com/adafruit/Adafruit_TCS34725 to communicate with the sensor. This library has a
    dependency on the arduino Wire library. By default, the Wire library communicates over I2C at 10Khz, but can be modified to run at 400Khz (max communication
    frequency supported by the sensor). To do this, close all instances of the arduino IDE. Then, navigate to the base directory of the arduino IDE (where the
    arduino executable is stored), and open hardware/arduino/avr/libraries/Wire/src/utility/twi.h. Change the value of TWI_FREQ from 100000L to 400000L. Then
    reopen the arduino IDE. Install the TCS library by navigating to the library github repository and selecting "Clone or download" -> "Download ZIP". Move the
    libray zip file to a known location. Then in the arduino IDE select "Sketch" -> "Include Library" -> "Add .ZIP Library..." then navigate to where the
    adafruit zip file is stored, select it, and click "OK". Open the sensor controller sketch and select "Verify" to recomile.
    Configuration of the wifi module is done via the arduino serial monitor using an arduino programed with a blank sketch as a bridge. First program the arduino with a blank sketch. Then, connect esp power to arduino 3.3V, esp ground to a power bus GND, esp RX to arduino RX (through the voltage divider), and esp TX to arduino TX. You should see a red LED on the esp light up indicating the esp is on. Open the arduino serial monitor and set the baud rate to the esp baud rate (our esps have a baud rate set to 57600, esps out of the box have a baud rate of 115200) and the line endings to "newline and carridge return" You can then type commands into the serial monitor and send them to the esp module.

AT Commands used:
- AT: checks system setup.
- AT+CIOBAUD=<BAUD>: sets baud rate at which esp module communicates to BAUD, persists after power cycle. Our esps have already been configured with this command to opperate at 56700 baud.
- AT+CWMODE=1: configures wifi module to be in station/client mode, persists after power cycle. Our esps have already been configured with this command.
- AT+CWLAP: list available networks
- AT+CWJAP="<name>","<password>": Join a network, persists after power cycle. Our esps have already been configured with this command to join the "penta-lab" network with penta-lab password.
- AT+RESTORE: restores esp module to factory settings
See full list of AT commands here: https://www.itead.cc/wiki/images/5/53/Esp8266_at_instruction_set_en_v1.5.4_0.pdf


## Troubleshooting
In the sensor controller has a DEBUG option. Set this to 1 when the sensor module is assembled and the arduino is connected to the arduino IDE. This will print sensor readings and esp responses on the serial monitor.
Extend Wi-Fi module wait time to fix hanging issue (module stops transmitting data):
Go to ./arduino/sensor_controller/sensor_controller.ino
Increase the delay time in lines 132 and 134; the numbers are in milliseconds
Reprogram each of the 12 Arduinos
If that doesn’t work, leave the system off for a couple of hours


