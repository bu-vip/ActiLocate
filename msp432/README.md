# Installation
To begin installation, we recommend you have git installed on our command line. From here clone our repository by typing git clone: “https://github.com/thewilliamchen/SeniorDesign.git”. 

Download Code Composer Studio v7 from ti, which can be found at http://www.ti.com/tool/CCSTUDIO. Once installed, open the program and go to File → Open Projects From File System and select the “msp” subdirectory within the github repository you just cloned. Edit the MSP432 main.c code to input your matrix.

## Changing the base clock frequency of the MSP
Lines 183 and 185 in main.c are the two clock dividers;
In line 183, set TIMER_A_CTL_ID_0/1/2/3 to divide the clock by 1/2/4/8;
In line 185, set TIMER_A_EX0_TAIDEX_0/1/2/3/4/5/6/7 to further divide the clock by 1/2/3/4/5/6/7/8;
The base clock frequency  is 3MHz divided by the product of the two clock dividers.

## Changing frequency that the state changes
To get the desired state change frequency output from the msp, divide the base ckock frequency by the desired frequency and set that value to state_change_freq.
Set state_duty to half of state_change_freq. The maximum value of state_change_freq is 65535 which places a lower limit on the frequeny that can generated this
way: to lower frequency further, lower the MSP base clock frequency


## Changing Input Matrix
Each row of the matrix represents a state and each column represents an LED. If an entry in the matrix is 1, it represents at that particular state, the LED is
on. This matrix must match the state matrix configured in the parsing server configuration file, config.py

## Switch
Because the on/off switch is a mechanical device, changing the switch state may not result in a clean voltage change on the MSP pin: there may be some "bounce"
or multiple pin state changes detected by the MSP. This would result in the patern turning on and off multiple times when the switch was flipped, turning on/off
when the wires were jiggled, and generally not behaving as expected. To solve this issue, we're debouncing the MSP in software. The switch is set to debounce
every 2 seconds. To allow it debounce quicker, decrease the value of DEBOUNCE_COUNT. A debounce counter is incremented every 5ms. So if you were to set the
debounce count to 40, then the button will debounce every .005 * 40 = 0.2s.

##Running the MSP
The MSP recieves power over USB from a wall adapter and should remain plugged in and powered on at all times to ensure a common ground is present for all
system components. The LEDs and arduinos can be powered on and off, but before powering the arduinos on, make sure the MSP clock signal/pattern is not being
generated. Syncronization across the arduinos is insured by having each arduino count the number of clock signals recieved and having the parsing server
generate state datasets from all data associated with a particular clock signal. The 0th clock signal is set when the arduino finishes booting and the arduinos
do not all take the same amount of time to boot, so if the patern is running while the arduinos are booting, different arduinos may set the 0th clock signal on
fifferent clock signals, resulting in data being out of sync. Avoid this by starting the arduinos, wating for them to fully boot, then turning on the clock
signal. 
