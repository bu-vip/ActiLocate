# Setup

please note: ubuntu with windows subsystem  and linux systems behave differently. You will have to first figure out which /dev/ files correspond to the arduinos and update them in read_serial_system.sh.

# How to use the USB Client Interface

navigate to usb client folder

`cd /path/to/repo/usb_serial`

to begin a connection to the usb devices, run the real_serial script for your appropriate system.

`./read_serial_<system>.sh`

turn on switch, run for some amount of time

wait for the usb devices to finish reading with an output that says "cycle complete" for all usb devices

turn off switch, then kill the read_serial.sh script (Ctrl-C)

run parser and generate output

`python parser.py`
