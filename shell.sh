#!/bin/bash

make
sudo rmmod charMod
sudo insmod charMod.ko
printf 'boxinIsTesting'>/dev/charMod
cat /dev/charMod
echo "finished"
