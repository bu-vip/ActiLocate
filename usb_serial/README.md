# Setup

please note: ubuntu with windows subsystem  and linux systems behave differently. You will have to first figure out which /dev/ files correspond to the arduinos and update them in read_serial_system.sh.

# How to use the USB Client Interface

navigate to usb client folder

`cd /path/to/repo/usb_serial`

to begin a connection to the usb devices, run the run_all script

`sudo ./run_all.sh`

turn on msp432

wait until you've captured all the cycles you want to. 

press "q" where the run_all.sh  script was run, and wait a bit

access output files in the output directory. Each file corresponds to one cycle,
with the exception of the first file.


