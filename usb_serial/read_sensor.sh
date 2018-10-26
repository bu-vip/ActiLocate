#!/bin/bash
rm -f $2
while true; do
./arduino-serial --eolchar "*" -b 57600 -p $1 -r --timeout 15000 >> $2
echo "read complete from" $1
done
