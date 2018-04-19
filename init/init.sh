#!/bin/sh

ifconfig eth0 164.125.70.173 broadcast 164.125.70.255 netmask 255.255.255.0
route add default gw 164.125.70.1
mount -t nfs 164.125.70.172:/nfsroot /mnt/nfs -o rw,rsize=4096,nolock
