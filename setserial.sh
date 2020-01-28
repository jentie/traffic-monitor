#!/bin/bash
#
# setserial - (re)set USB interface, esp. without using Arduino IDE
#

if [ $# != 1 ]; then
    echo "usage: setserial /dev/ttyS?"
    exit 1
fi

echo "initialize interface" $1

stty -F $1 115200 
stty -F $1 -crtscts -ixon

exit 0
