#!/bin/bash
rm -f $2
./arduino-serial --eolchar "*" -b 57600 -p $1 -r --timeout 15000 >> $2
echo "read complete from" $1
