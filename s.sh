#!/bin/bash

make
sudo rmmod charMod
sudo insmod charMod.ko
#echo "cat /dev/charMod"
#cat /dev/charMod

#echo "printf boxin > /dev/charMod"
#printf boxin > /dev/charMod
#cat /dev/charMod

echo "compiling testing.cpp"
make testing

echo "running testing"
sudo ./testing
echo "finished"
