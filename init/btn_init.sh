#!/bin/sh

bluetoothd
hciconfig hci0 up
hciconfig hci0 piscan
sdptool add --channel=1 SP
rfcomm listen 0 1&

