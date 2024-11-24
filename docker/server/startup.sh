#!/bin/bash

ip addr add 192.168.1.1/24 dev lo
ip addr add 192.168.1.2/24 dev lo

ip link set lo up

echo "Setup executed"

/usr/local/bin/myserver
