#!/bin/bash

date

echo "$ ifconfig"
ifconfig

#echo "$ sudo nmap -O 192.168.0.107/24"
#sudo nmap -A 192.168.0.107/24

echo "$ sudo nmap -O 192.168.0.*"
sudo nmap -A 192.168.0.*
#sudo nmap -A 192.168.0.0-255


